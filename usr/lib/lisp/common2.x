(File common2.l)
(sys:gethostname lambda int:franz-call)
(filestat lambda error int:franz-call null tilde-expand setq stringp symbolp or cond let)
(sys:getpid lambda int:franz-call)
(sys:chmod lambda error bigp not cdr null int:franz-call zerop tilde-expand setq fixp symbolp stringp or and cond)
(sys:time lambda int:franz-call)
(sys:link lambda error bigp not cdr null and int:franz-call zerop tilde-expand setq stringp symbolp or cond)
(sys:unlink lambda error bigp not cdr null and int:franz-call zerop tilde-expand setq stringp symbolp or cond)
(chdir lambda error bigp not cdr null and int:franz-call zerop tilde-expand setq stringp symbolp or cond)
(sys:access lambda error bigp not cdr null int:franz-call zerop tilde-expand setq fixp stringp symbolp or and cond)
(sys:getpwnam-dir lambda vref)
(sys:getpwnam lambda error int:franz-call stringp symbolp or cond)
(username-to-dir-flush-cache lambda setq)
(username-to-dir lambda sys:getpwnam-dir cdr cons setf push sys:getpwnam null assoc let error stringp get_pname setq symbolp cond)
(probef lambda sys:access error stringp symbolp or not cond)
(cfasl lambda quote lexpr-funcall tilde-expand setq error stringp symbolp or not cond)
(fasl lambda quote lexpr-funcall tilde-expand setq error stringp symbolp or not cond)
(fileopen lambda int:fileopen tilde-expand setq error stringp symbolp or not cond)
(outfile lambda int:outfile tilde-expand setq error stringp symbolp or not cond)
(infile lambda int:infile tilde-expand setq error stringp symbolp or not cond)
(environment-lmlisp macro append quote cons cdr)
(environment-maclisp macro append quote cons cdr)
(environment-syntax nlambda cdr error cvttofranzlisp memq cvttoucilisp cvttointlisp cvttomaclisp eq cond caseq car null setq liszt-internal-do mapc)
(environment-files nlambda cdr load quote get not cond if car null setq liszt-internal-do mapc)
(environment macro cddr list setf push concat symbolp dtpr and ncons atom cond if cadr car setq nreverse append quote cons null do cdr)
(subst-eqp lambda cdr car subst-eqp or dtpr eq cond)
(subst-eq lambda cdr subst-eq cons car subst-eqp dtpr and eq cond)
(trace-funp lambda caadar caar quote eq car cond dtpr memq symbolp and or)
(baktraceprint lambda car dtpr let print atom terpr nwritn cdr >& > patom not cond)
(showstack-baktrace lambda |1-| baktraceprint Internal-bcdcall getdisc bcdp cxr getd symbolp funcall quote subst-eq trace-funp return int:showstack terpr bigp not and zerop equal or cdr fixp setq car eq cond null do let)
(baktrace nlambda showstack-baktrace)
(showstack nlambda showstack-baktrace)
(filestat:ino lambda quote concat error vref vectorp cond)
(filestat:rdev lambda quote concat error vref vectorp cond)
(filestat:dev lambda quote concat error vref vectorp cond)
(filestat:ctime lambda quote concat error vref vectorp cond)
(filestat:mtime lambda quote concat error vref vectorp cond)
(filestat:atime lambda quote concat error vref vectorp cond)
(filestat:size lambda quote concat error vref vectorp cond)
(filestat:gid lambda quote concat error vref vectorp cond)
(filestat:uid lambda quote concat error vref vectorp cond)
(filestat:nlink lambda quote concat error vref vectorp cond)
(filestat:type lambda quote concat error vref vectorp cond)
(filestat:mode lambda quote concat error vref vectorp cond)
(int:wrong-number-of-args-error lexpr cons setf push error length format setq eq =& cond if <& do - |1-| cdr arg let)
(int:vector-range-error lambda list error)
(pop macro quote list car cddr setq cond caddr cadr)
(push macro quote list caddr cadr)
(g00644::args lambda cadr quote list)
(g00638::arg lambda cadr quote list)
(g00632::symeval lambda cadr quote list)
(g00626::plist lambda cadr quote list)
(g00620::get lambda caddr cadr quote list)
(g00614::arraycall lambda quote list)
(g00608::nthcdr lambda caddr cadr quote list)
(g00602::nth lambda caddr cadr quote list)
(g00596::vrefi-long lambda caddr cadr quote list)
(g00590::vrefi-word lambda caddr cadr quote list)
(g00584::vrefi-byte lambda caddr cadr quote list)
(g00578::vref lambda caddr cadr quote list)
(g00572::cxr lambda caddr cadr quote list)
(g00566::cddr lambda cadr quote list)
(g00560::cdar lambda cadr quote list)
(g00554::cdr lambda cadr quote list)
(g00548::cadr lambda cadr quote list)
(g00542::caar lambda cadr quote list)
(g00536::car lambda cadr quote list)
(defsetf macro cons quote list cdddr caddr cadr)
(setf-check-cad+r lambda return memq not append implode ascii concat cons quote list eval setq null do car exploden cdr nreverse let getcharn eq cond if)
(setf macro getdisc bcdp eq dtpr cxr getd setf-check-cad+r append cons apply return get setq and car do list quote error symbolp or atom cond cdddr caddr cadr)
(tconc lambda rplaca cdr rplacd dtpr ncons setq cons atom cond)
(tailp lambda go cdr setq eq return atom cond prog and)
(subpr lambda quote and cdar caar null cons eq not neq or return car subpr cdr setq go atom cond prog)
(subpair lambda quote or subpr cond)
(remove lambda |1-| rplacd list bigp cdr and zerop car equal not or cond null do quote let setq)
(quote!-expr-mac lambda cdr cddr quote!-expr-mac cadr car eq quote list atom null cond)
(quote! macro quote!-expr-mac cdr)
(neq macro quote list caddr cadr)
(merge1 lambda go return rplacd cdr prog1 Internal-bcdcall getdisc quote eq bcdp cxr getd symbolp and car funcall setq prog null cond declare)
(merge lambda merge1 quote function setq null cond declare)
(lsubst lambda cons cdr lsubst copy nconc car equal eq atom null cond)
(ldiff lambda go rplacd error return cdr car cons ncons setq prog null eq cond)
(lconc lambda rplaca cdr rplacd dtpr cons last setq atom cond return prog)
(kwote lambda quote list)
(insert lambda - go sub1 cddr cadr rplaca cons rplacd equal not Internal-bcdcall getdisc eq bcdp cxr getd symbolp and car funcall <& < Cnth add1 cdr / length prog quote function setq error atom list null cond)
(dsubst lambda go cdr rplacd and dsubst rplaca equal car symbolp atom copy return setq eq cond prog)
(Cnth lambda go |1-| cdr setq return eq atom or prog cons >& > cond)
(attach lambda error rplaca cdr car cons rplacd dtpr cond)
($patom1 lambda patom)
(charcnt lambda nwritn cdr -)
(tab lexpr - printblanks terpri nwritn cdr >& cond arg setq prog)
(remq lambda cdr |1-| rplacd list =& car eq not or cond null do let setq)
(listp lambda null dtpr or)
(nth::cmacro:g00175 macro quote list caddr cadr)
(nth lambda nthcdr car)
(nthcdr::cmacro:g00156 macro list cdr car return caar null quote do assq let eq =& fixp and cond if caddr cadr)
(nthcdr lambda error cdr |1-| nthcdr eq =& cons <& fixp cond)
(charcnt lambda nwritn cdr -)
(linelength nlambda setq car numberp null cond)
(printblanks lambda |1-| <& do cadr patom cond memq let)
(msg-tyo-char lambda |1-| tyo terpr eq cond <& < do)
(msg macro ncons nconc cadr list eq dtpr cond If car setq nreverse append quote cons null do cdr)
(process nlambda set *process cdr null cond caddr cadr car let declare)
(*process-receive lambda *process car)
(*process-send lambda *process cadr)
