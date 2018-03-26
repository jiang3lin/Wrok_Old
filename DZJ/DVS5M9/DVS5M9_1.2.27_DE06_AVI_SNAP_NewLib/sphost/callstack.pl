#!/usr/bin/perl

my $argc = @ARGV;

if ($argc > 0) {
	$log = $ARGV[0];
}
else {
	print "Usage: chklog.pl logfile\n";
	exit();
}

chomp(my $prj = `./helper.sh project`);
chomp(my $chip = `./helper.sh chip`);
#print "_$prj/spca$chip.elf";

open(ELF, "../tool/objdump -t _$prj/spca$chip.elf | grep -w \'\\.\\(text\\|rom\\)\\(\\.${w}\\+\\)\\?\' | cut -c 1-9,32- | sort |") || die "Can't dump elf file\n";
my @dmp = <ELF>;
close(ELF);
#print @dmp;

open(LOG, "< $log") || die "Can't open log file\n";
while (<LOG>) {
	print ;
	if (/.*addr\.sh((\s\w{8})+)\s.*/) {
		#print $1,"\n";
		open(CALLSTACK, "| ../tool/addr2func.exe $1 | sort");
		print CALLSTACK @dmp;
		close(CALLSTACK);
	}
}
close(LOG);
