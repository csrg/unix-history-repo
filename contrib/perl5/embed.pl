#!/usr/bin/perl -w

require 5.003;

# XXX others that may need adding
#       warnhook
#       hints
#       copline
my @extvars = qw(sv_undef sv_yes sv_no na dowarn
                 curcop compiling 
                 tainting tainted stack_base stack_sp sv_arenaroot
                 curstash DBsub DBsingle debstash
                 rsfp 
                 stdingv
		 defgv
		 errgv
		 rsfp_filters
		 perldb
		 diehook
		 dirty
		 perl_destruct_level
                );

sub readsyms (\%$) {
    my ($syms, $file) = @_;
    %$syms = ();
    local (*FILE, $_);
    open(FILE, "< $file")
	or die "embed.pl: Can't open $file: $!\n";
    while (<FILE>) {
	s/[ \t]*#.*//;		# Delete comments.
	if (/^\s*(\S+)\s*$/) {
	    $$syms{$1} = 1;
	}
    }
    close(FILE);
}

readsyms %global, 'global.sym';
readsyms %interp, 'interp.sym';

sub readvars(\%$$) {
    my ($syms, $file,$pre) = @_;
    %$syms = ();
    local (*FILE, $_);
    open(FILE, "< $file")
	or die "embed.pl: Can't open $file: $!\n";
    while (<FILE>) {
	s/[ \t]*#.*//;		# Delete comments.
	if (/PERLVARI?C?\($pre(\w+)/) {
	    $$syms{$1} = 1;
	}
    }
    close(FILE);
}

my %intrp;
my %thread;

readvars %intrp,  'intrpvar.h','I';
readvars %thread, 'thrdvar.h','T';
readvars %globvar, 'perlvars.h','G';

foreach my $sym (sort keys %intrp)
 {
  warn "$sym not in interp.sym\n" unless exists $interp{$sym};
  if (exists $global{$sym})
   {
    delete $global{$sym};
    warn "$sym in global.sym as well as interp\n";
   }
 }

foreach my $sym (sort keys %globvar)
 {
  if (exists $global{$sym})
   {
    delete $global{$sym};
    warn "$sym in global.sym as well as perlvars.h\n";
   }
 }

foreach my $sym (keys %interp)
 {
  warn "extra $sym in interp.sym\n" 
   unless exists $intrp{$sym} || exists $thread{$sym};
 }

foreach my $sym (sort keys %thread)
 {
  warn "$sym in intrpvar.h\n" if exists $intrp{$sym};
  if (exists $global{$sym})
   {
    delete $global{$sym};
    warn "$sym in global.sym as well as thread\n";
   }
 }

sub hide ($$) {
    my ($from, $to) = @_;
    my $t = int(length($from) / 8);
    "#define $from" . "\t" x ($t < 3 ? 3 - $t : 1) . "$to\n";
}
sub embed ($) {
    my ($sym) = @_;
    hide($sym, "Perl_$sym");
}
sub embedvar ($) {
    my ($sym) = @_;
#   hide($sym, "Perl_$sym");
    return '';
}

sub multon ($$$) {
    my ($sym,$pre,$ptr) = @_;
    hide("PL_$sym", "($ptr$pre$sym)");
}
sub multoff ($$) {
    my ($sym,$pre) = @_;
    return hide("PL_$pre$sym", "PL_$sym");
}

unlink 'embed.h';
open(EM, '> embed.h')
    or die "Can't create embed.h: $!\n";

print EM <<'END';
/* !!!!!!!   DO NOT EDIT THIS FILE   !!!!!!! 
   This file is built by embed.pl from global.sym, intrpvar.h,
   and thrdvar.h.  Any changes made here will be lost!
*/

/* (Doing namespace management portably in C is really gross.) */

/*  EMBED has no run-time penalty, but helps keep the Perl namespace
    from colliding with that used by other libraries pulled in
    by extensions or by embedding perl.  Allow a cc -DNO_EMBED
    override, however, to keep binary compatability with previous
    versions of perl.
*/
#ifndef NO_EMBED
#  define EMBED 1 
#endif

/* Hide global symbols? */

#ifdef EMBED

END

for $sym (sort keys %global) {
    print EM embed($sym);
}

print EM <<'END';

#endif /* EMBED */

END

close(EM);

unlink 'embedvar.h';
open(EM, '> embedvar.h')
    or die "Can't create embedvar.h: $!\n";

print EM <<'END';
/* !!!!!!!   DO NOT EDIT THIS FILE   !!!!!!! 
   This file is built by embed.pl from global.sym, intrpvar.h,
   and thrdvar.h.  Any changes made here will be lost!
*/

/* (Doing namespace management portably in C is really gross.) */

/*  EMBED has no run-time penalty, but helps keep the Perl namespace
    from colliding with that used by other libraries pulled in
    by extensions or by embedding perl.  Allow a cc -DNO_EMBED
    override, however, to keep binary compatability with previous
    versions of perl.
*/


/* Put interpreter-specific symbols into a struct? */

#ifdef MULTIPLICITY

#ifndef USE_THREADS
/* If we do not have threads then per-thread vars are per-interpreter */

END

for $sym (sort keys %thread) {
    print EM multon($sym,'T','PL_curinterp->');
}

print EM <<'END';

#endif /* !USE_THREADS */

/* These are always per-interpreter if there is more than one */

END

for $sym (sort keys %intrp) {
    print EM multon($sym,'I','PL_curinterp->');
}

print EM <<'END';

#else	/* !MULTIPLICITY */

END

for $sym (sort keys %intrp) {
    print EM multoff($sym,'I');
}

print EM <<'END';

#ifndef USE_THREADS

END

for $sym (sort keys %thread) {
    print EM multoff($sym,'T');
}

print EM <<'END';

#endif /* USE_THREADS */

/* Hide what would have been interpreter-specific symbols? */

#ifdef EMBED

END

for $sym (sort keys %intrp) {
    print EM embedvar($sym);
}

print EM <<'END';

#ifndef USE_THREADS

END

for $sym (sort keys %thread) {
    print EM embedvar($sym);
}

print EM <<'END';

#endif /* USE_THREADS */
#endif /* EMBED */
#endif /* MULTIPLICITY */

/* Now same trickey for per-thread variables */

#ifdef USE_THREADS

END

for $sym (sort keys %thread) {
    print EM multon($sym,'T','thr->');
}

print EM <<'END';

#endif /* USE_THREADS */

#ifdef PERL_GLOBAL_STRUCT

END

for $sym (sort keys %globvar) {
    print EM multon($sym,'G','PL_Vars.');
}

print EM <<'END';

#else /* !PERL_GLOBAL_STRUCT */

END

for $sym (sort keys %globvar) {
    print EM multoff($sym,'G');
}

print EM <<'END';

#ifdef EMBED

END

for $sym (sort keys %globvar) {
    print EM embedvar($sym);
}

print EM <<'END';

#endif /* EMBED */
#endif /* PERL_GLOBAL_STRUCT */

END

print EM <<'END';

#ifndef MIN_PERL_DEFINE  

END

for $sym (sort @extvars) {
    print EM hide($sym,"PL_$sym");
}

print EM <<'END';

#endif /* MIN_PERL_DEFINE */
END


close(EM);
