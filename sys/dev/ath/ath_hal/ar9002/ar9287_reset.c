/*
 * Copyright (c) 2002-2009 Sam Leffler, Errno Consulting
 * Copyright (c) 2002-2008 Atheros Communications, Inc.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * $FreeBSD$
 */

#include "opt_ah.h"

#include "ah.h"
#include "ah_internal.h"
#include "ah_devid.h"

#include "ah_eeprom_v14.h"
#include "ah_eeprom_9287.h"

#include "ar5416/ar5416.h"
#include "ar5416/ar5416reg.h"
#include "ar5416/ar5416phy.h"

#include "ar9002/ar9287phy.h"
#include "ar9002/ar9287an.h"

#include "ar9002/ar9287_olc.h"
#include "ar9002/ar9287_reset.h"

/*
 * Set the TX power calibration table per-chain.
 *
 * This only supports open-loop TX power control for the AR9287.
 */
static void
ar9287SetPowerCalTable(struct ath_hal *ah,
    const struct ieee80211_channel *chan, int16_t *pTxPowerIndexOffset)
{
        struct cal_data_op_loop_ar9287 *pRawDatasetOpenLoop;
        uint8_t *pCalBChans = NULL;
        uint16_t pdGainOverlap_t2;
        uint16_t numPiers = 0, i;
        uint16_t numXpdGain, xpdMask;
        uint16_t xpdGainValues[AR5416_NUM_PD_GAINS] = {0, 0, 0, 0};
        uint32_t regChainOffset;
	HAL_EEPROM_9287 *ee = AH_PRIVATE(ah)->ah_eeprom;
        struct ar9287_eeprom *pEepData = &ee->ee_base;

        xpdMask = pEepData->modalHeader.xpdGain;

        if ((pEepData->baseEepHeader.version & AR9287_EEP_VER_MINOR_MASK) >=
            AR9287_EEP_MINOR_VER_2)
                pdGainOverlap_t2 = pEepData->modalHeader.pdGainOverlap;
        else
                pdGainOverlap_t2 = (uint16_t)(MS(OS_REG_READ(ah, AR_PHY_TPCRG5),
                                            AR_PHY_TPCRG5_PD_GAIN_OVERLAP));

	/* Note: Kiwi should only be 2ghz.. */
        if (IEEE80211_IS_CHAN_2GHZ(chan)) {
                pCalBChans = pEepData->calFreqPier2G;
                numPiers = AR9287_NUM_2G_CAL_PIERS;
                pRawDatasetOpenLoop = (struct cal_data_op_loop_ar9287 *)pEepData->calPierData2G[0];
                AH5416(ah)->initPDADC = pRawDatasetOpenLoop->vpdPdg[0][0];
        }
        numXpdGain = 0;

        /* Calculate the value of xpdgains from the xpdGain Mask */
        for (i = 1; i <= AR5416_PD_GAINS_IN_MASK; i++) {
                if ((xpdMask >> (AR5416_PD_GAINS_IN_MASK - i)) & 1) {
                        if (numXpdGain >= AR5416_NUM_PD_GAINS)
                                break;
                        xpdGainValues[numXpdGain] =
                                (uint16_t)(AR5416_PD_GAINS_IN_MASK-i);
                        numXpdGain++;
                }
        }

        OS_REG_RMW_FIELD(ah, AR_PHY_TPCRG1, AR_PHY_TPCRG1_NUM_PD_GAIN,
                      (numXpdGain - 1) & 0x3);
        OS_REG_RMW_FIELD(ah, AR_PHY_TPCRG1, AR_PHY_TPCRG1_PD_GAIN_1,
                      xpdGainValues[0]);
        OS_REG_RMW_FIELD(ah, AR_PHY_TPCRG1, AR_PHY_TPCRG1_PD_GAIN_2,
                      xpdGainValues[1]);
        OS_REG_RMW_FIELD(ah, AR_PHY_TPCRG1, AR_PHY_TPCRG1_PD_GAIN_3,
                      xpdGainValues[2]);

        for (i = 0; i < AR9287_MAX_CHAINS; i++) {
                regChainOffset = i * 0x1000;

                if (pEepData->baseEepHeader.txMask & (1 << i)) {
                        int8_t txPower;
                        pRawDatasetOpenLoop =
                        (struct cal_data_op_loop_ar9287 *)pEepData->calPierData2G[i];
                                ar9287olcGetTxGainIndex(ah, chan,
                                    pRawDatasetOpenLoop,
                                    pCalBChans, numPiers,
                                    &txPower);
                                ar9287olcSetPDADCs(ah, txPower, i);
                }
        }

        *pTxPowerIndexOffset = 0;
}

HAL_BOOL
ar9287SetTransmitPower(struct ath_hal *ah,
	const struct ieee80211_channel *chan, uint16_t *rfXpdGain)
{
	int16_t txPowerIndexOffset = 0;

	/* XXX TODO */

	/* Fetch per-rate power table for the given channel */

	/* Set open-loop TX power control calibration */
	ar9287SetPowerCalTable(ah, chan, &txPowerIndexOffset);

	/* Calculate regulatory maximum power level */

	/* Kiwi TX power starts at -5 dBm */

	/* Write TX power registers */

	return AH_TRUE;
}

/*
 * Read EEPROM header info and program the device for correct operation
 * given the channel value.
 */
HAL_BOOL
ar9287SetBoardValues(struct ath_hal *ah, const struct ieee80211_channel *chan)
{
	const HAL_EEPROM_9287 *ee = AH_PRIVATE(ah)->ah_eeprom;
	const struct ar9287_eeprom *eep = &ee->ee_base;
	const struct modal_eep_ar9287_header *pModal = &eep->modalHeader;
	uint16_t antWrites[AR9287_ANT_16S];
	uint32_t regChainOffset, regval;
	uint8_t txRxAttenLocal;
	int i, j, offset_num;

	pModal = &eep->modalHeader;

	antWrites[0] = (uint16_t)((pModal->antCtrlCommon >> 28) & 0xF);
	antWrites[1] = (uint16_t)((pModal->antCtrlCommon >> 24) & 0xF);
	antWrites[2] = (uint16_t)((pModal->antCtrlCommon >> 20) & 0xF);
	antWrites[3] = (uint16_t)((pModal->antCtrlCommon >> 16) & 0xF);
	antWrites[4] = (uint16_t)((pModal->antCtrlCommon >> 12) & 0xF);
	antWrites[5] = (uint16_t)((pModal->antCtrlCommon >> 8) & 0xF);
	antWrites[6] = (uint16_t)((pModal->antCtrlCommon >> 4)  & 0xF);
	antWrites[7] = (uint16_t)(pModal->antCtrlCommon & 0xF);

	offset_num = 8;

	for (i = 0, j = offset_num; i < AR9287_MAX_CHAINS; i++) {
		antWrites[j++] = (uint16_t)((pModal->antCtrlChain[i] >> 28) & 0xf);
		antWrites[j++] = (uint16_t)((pModal->antCtrlChain[i] >> 10) & 0x3);
		antWrites[j++] = (uint16_t)((pModal->antCtrlChain[i] >> 8) & 0x3);
		antWrites[j++] = 0;
		antWrites[j++] = (uint16_t)((pModal->antCtrlChain[i] >> 6) & 0x3);
		antWrites[j++] = (uint16_t)((pModal->antCtrlChain[i] >> 4) & 0x3);
		antWrites[j++] = (uint16_t)((pModal->antCtrlChain[i] >> 2) & 0x3);
		antWrites[j++] = (uint16_t)(pModal->antCtrlChain[i] & 0x3);
	}

	OS_REG_WRITE(ah, AR_PHY_SWITCH_COM, pModal->antCtrlCommon);

	for (i = 0; i < AR9287_MAX_CHAINS; i++)	{
		regChainOffset = i * 0x1000;

		OS_REG_WRITE(ah, AR_PHY_SWITCH_CHAIN_0 + regChainOffset,
			  pModal->antCtrlChain[i]);

		OS_REG_WRITE(ah, AR_PHY_TIMING_CTRL4_CHAIN(0) + regChainOffset,
			  (OS_REG_READ(ah, AR_PHY_TIMING_CTRL4_CHAIN(0) + regChainOffset)
			   & ~(AR_PHY_TIMING_CTRL4_IQCORR_Q_Q_COFF |
			       AR_PHY_TIMING_CTRL4_IQCORR_Q_I_COFF)) |
			  SM(pModal->iqCalICh[i],
			     AR_PHY_TIMING_CTRL4_IQCORR_Q_I_COFF) |
			  SM(pModal->iqCalQCh[i],
			     AR_PHY_TIMING_CTRL4_IQCORR_Q_Q_COFF));

		txRxAttenLocal = pModal->txRxAttenCh[i];

		OS_REG_RMW_FIELD(ah, AR_PHY_GAIN_2GHZ + regChainOffset,
			      AR_PHY_GAIN_2GHZ_XATTEN1_MARGIN,
			      pModal->bswMargin[i]);
		OS_REG_RMW_FIELD(ah, AR_PHY_GAIN_2GHZ + regChainOffset,
			      AR_PHY_GAIN_2GHZ_XATTEN1_DB,
			      pModal->bswAtten[i]);
		OS_REG_RMW_FIELD(ah, AR_PHY_RXGAIN + regChainOffset,
			      AR9280_PHY_RXGAIN_TXRX_ATTEN,
			      txRxAttenLocal);
		OS_REG_RMW_FIELD(ah, AR_PHY_RXGAIN + regChainOffset,
			      AR9280_PHY_RXGAIN_TXRX_MARGIN,
			      pModal->rxTxMarginCh[i]);
	}


	if (IEEE80211_IS_CHAN_HT40(chan))
		OS_REG_RMW_FIELD(ah, AR_PHY_SETTLING,
			      AR_PHY_SETTLING_SWITCH, pModal->swSettleHt40);
	else
		OS_REG_RMW_FIELD(ah, AR_PHY_SETTLING,
			      AR_PHY_SETTLING_SWITCH, pModal->switchSettling);

	OS_REG_RMW_FIELD(ah, AR_PHY_DESIRED_SZ,
		      AR_PHY_DESIRED_SZ_ADC, pModal->adcDesiredSize);

	OS_REG_WRITE(ah, AR_PHY_RF_CTL4,
		  SM(pModal->txEndToXpaOff, AR_PHY_RF_CTL4_TX_END_XPAA_OFF)
		  | SM(pModal->txEndToXpaOff, AR_PHY_RF_CTL4_TX_END_XPAB_OFF)
		  | SM(pModal->txFrameToXpaOn, AR_PHY_RF_CTL4_FRAME_XPAA_ON)
		  | SM(pModal->txFrameToXpaOn, AR_PHY_RF_CTL4_FRAME_XPAB_ON));

	OS_REG_RMW_FIELD(ah, AR_PHY_RF_CTL3,
		      AR_PHY_TX_END_TO_A2_RX_ON, pModal->txEndToRxOn);

	OS_REG_RMW_FIELD(ah, AR_PHY_CCA,
		      AR9280_PHY_CCA_THRESH62, pModal->thresh62);
	OS_REG_RMW_FIELD(ah, AR_PHY_EXT_CCA0,
		      AR_PHY_EXT_CCA0_THRESH62, pModal->thresh62);

	regval = OS_REG_READ(ah, AR9287_AN_RF2G3_CH0);
	regval &= ~(AR9287_AN_RF2G3_DB1 |
		    AR9287_AN_RF2G3_DB2 |
		    AR9287_AN_RF2G3_OB_CCK |
		    AR9287_AN_RF2G3_OB_PSK |
		    AR9287_AN_RF2G3_OB_QAM |
		    AR9287_AN_RF2G3_OB_PAL_OFF);
	regval |= (SM(pModal->db1, AR9287_AN_RF2G3_DB1) |
		   SM(pModal->db2, AR9287_AN_RF2G3_DB2) |
		   SM(pModal->ob_cck, AR9287_AN_RF2G3_OB_CCK) |
		   SM(pModal->ob_psk, AR9287_AN_RF2G3_OB_PSK) |
		   SM(pModal->ob_qam, AR9287_AN_RF2G3_OB_QAM) |
		   SM(pModal->ob_pal_off, AR9287_AN_RF2G3_OB_PAL_OFF));

	OS_REG_WRITE(ah, AR9287_AN_RF2G3_CH0, regval);
	OS_DELAY(100);	/* analog write */

	regval = OS_REG_READ(ah, AR9287_AN_RF2G3_CH1);
	regval &= ~(AR9287_AN_RF2G3_DB1 |
		    AR9287_AN_RF2G3_DB2 |
		    AR9287_AN_RF2G3_OB_CCK |
		    AR9287_AN_RF2G3_OB_PSK |
		    AR9287_AN_RF2G3_OB_QAM |
		    AR9287_AN_RF2G3_OB_PAL_OFF);
	regval |= (SM(pModal->db1, AR9287_AN_RF2G3_DB1) |
		   SM(pModal->db2, AR9287_AN_RF2G3_DB2) |
		   SM(pModal->ob_cck, AR9287_AN_RF2G3_OB_CCK) |
		   SM(pModal->ob_psk, AR9287_AN_RF2G3_OB_PSK) |
		   SM(pModal->ob_qam, AR9287_AN_RF2G3_OB_QAM) |
		   SM(pModal->ob_pal_off, AR9287_AN_RF2G3_OB_PAL_OFF));

	OS_REG_WRITE(ah, AR9287_AN_RF2G3_CH1, regval);
	OS_DELAY(100);	/* analog write */

	OS_REG_RMW_FIELD(ah, AR_PHY_RF_CTL2,
	    AR_PHY_TX_FRAME_TO_DATA_START, pModal->txFrameToDataStart);
	OS_REG_RMW_FIELD(ah, AR_PHY_RF_CTL2,
	    AR_PHY_TX_FRAME_TO_PA_ON, pModal->txFrameToPaOn);

	OS_A_REG_RMW_FIELD(ah, AR9287_AN_TOP2,
	    AR9287_AN_TOP2_XPABIAS_LVL, pModal->xpaBiasLvl);

	return AH_TRUE;
}
