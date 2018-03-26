#!/usr/bin/perl

open(FILE, $ARGV[0]) || die "Cannot open $ARGV[0] (please input related pathname)\n";

%hash = ();

# $fmt=0; ",.text,.rodata,.data,.sdata,.sbss,.bss,romSize,ramSize\n";
# $fmt=1; ",.text,.rodata,.data,.sdata,.sbss,.bss,.pdr(X),.stab(X),.stabstr(X),totalSize,romSize,ramSize\n";
$fmt=0;

while(<FILE>)
{
	# search (/ /), if leading chars of line is space (/^\s*), a '.' (\.) append(.) with 1 or more chars, spaces (\s*), 0x, 8 word, spaces (\s*), 0x, 1 or more word, spaces (\s*), string with .a as postfix.
	# the string in 1st ( ) will be $1
	# the string in 2nd ( ) will be $2
	# the string in 3rd ( ) will be $3
	if (/^\s*(\..+?)\s*0x\w{8}\s*(0x.+?)\s+?(.+?\.a)/)
	{
		#Example in spca6330.map
		#" .text          0x80004b10       0x44 _RDK/objs/customization/HostFw/libhostfw.a(app_ver.o)"
		#" .rodata        0x802f167c       0x14 _RDK/objs/customization/HostFw/libhostfw.a(app_ver.o)"
		#" .data          0x80339f60       0x58 _RDK/objs/customization/HostFw/libhostfw.a(app_msg2key.o)"
		#" .sdata         0x80367630        0x4 _RDK/objs/customization/HostFw/libhostfw.a(app_power_on.o)"
		#" .sbss          0x8036b62c        0xc _RDK/objs/customization/HostFw/libhostfw.a(app_main.o)"
		#" .bss           0x8036f230       0x10 _RDK/objs/customization/HostFw/libhostfw.a(app_ver.o)"
		$section = $1;
		$size = hex($2);
		$lib = $3;					#"_RDK/objs/customization/HostFw/libhostfw.a"
		$lib =~ s/\\/\//g;			#"_RDK/objs/customization/HostFw/libhostfw.a" (replace any '\' to '/')
		$lib =~ s/.+\/(.+\.a)/$1/;	#"libhostfw.a" (strip to pathname to "xxxx.a")
		
		#print "$section, $size, $lib\n";
		$hash->{$lib}->{$section} += $size;
	}
}
close(FILE);

#
# Save Total Size first
#
foreach $libs (keys (%{$hash}))
{
	my ($allsize, $romsize, $ramsize, $sections);
	$allsize = 0;
	$romsize = 0;
	$ramsize = 0;
	#	
	# Sub-total size in ROM (binsize) and all size in section (size)
	#
	foreach $sections (keys (%{$hash->{$libs}}))
	{
		#print "$libs, $sections, $hash->{$libs}->{$sections}\n";
		$allsize += $hash->{$libs}->{$sections};
		if (($sections eq ".rom")
			|| ($sections eq ".text")
			|| ($sections eq ".data")
			|| ($sections eq ".jcr")
			|| ($sections eq ".ram_program")
			|| ($sections eq ".cri_sect")
			|| ($sections eq ".eh_frame")
			|| ($sections eq ".init")
			|| ($sections eq ".fini")
			|| ($sections eq ".ctors")
			|| ($sections eq ".dtors")
			|| ($sections eq ".rodata")
			|| ($sections eq ".sdata"))
		{
			$romsize += $hash->{$libs}->{$sections};
		}
	}
	$ramsize = $romsize + $hash->{$libs}->{".bss"} + $hash->{$libs}->{".sbss"};
	#print "$libs, $allsize, $romsize, $ramsize\n";
	$hash->{$libs}->{"totalSize"} = $allsize;
	$hash->{$libs}->{"romSize"} = $romsize;
	$hash->{$libs}->{"ramSize"} = $ramsize;
}

if ($fmt==0) {
	print "lib name,.text,.rodata,.data,.sdata,.sbss,.bss,romSize,ramSize,\n";
}
else {
	print "lib name,.text,.rodata,.data,.sdata,.sbss,.bss,.pdr(X),.stab(X),.stabstr(X),totalSize,romSize,ramSize,\n";
}

foreach $libs (sort by_size keys (%{$hash}))
{
	print "$libs,";
	print $hash->{$libs}->{".text"},",";
	print $hash->{$libs}->{".rodata"},",",$hash->{$libs}->{".data"},",",$hash->{$libs}->{".sdata"},",";
	print $hash->{$libs}->{".sbss"},",",$hash->{$libs}->{".bss"},",";
	if ($fmt==0) {
		print $hash->{$libs}->{"romSize"},",",$hash->{$libs}->{"ramSize"},",\n";
	}
	else {
		print $hash->{$libs}->{".pdr"},",",$hash->{$libs}->{".stab"},",",$hash->{$libs}->{".stabstr"},",";
		print $hash->{$libs}->{"totalSize"},",",$hash->{$libs}->{"romSize"},",",$hash->{$libs}->{"ramSize"},",\n";
	}
}

$totalSectionSize = 0;
$totalRomSize = 0;
$totalRamSize = 0;
foreach $libs (keys (%{$hash}))
{
	$totalSectionSize += $hash->{$libs}->{"totalSize"};
	$totalRomSize += $hash->{$libs}->{"romSize"};
	$totalRamSize += $hash->{$libs}->{"ramSize"};
}

if ($fmt==0) {
	my $romKB = sprintf "%.2f", $totalRomSize/1024;
	my $ramKB = sprintf "%.2f", $totalRamSize/1024;
	my $romMB = sprintf "%.2f", $totalRomSize/1048576;
	my $ramMB = sprintf "%.2f", $totalRamSize/1048576;
	print ",.text,.rodata,.data,.sdata,.sbss,.bss,romSize,ramSize,\n";
	print ",,,,,,,", $totalRomSize, ",",  $totalRamSize, ", Bytes,\n";
	print ",,,,,,,", $romKB, ",",  $ramKB, ", KBytes,\n";
	print ",,,,,,,", $romMB, ",",  $ramMB, ", MBytes,\n";
}
else {
	my $secKB = sprintf "%.2f", $totalSectionSize/1024;
	my $ramKB = sprintf "%.2f", $totalRamSize/1024;
	my $romKB = sprintf "%.2f", $totalRomSize/1024;
	my $secMB = sprintf "%.2f", $totalSectionSize/1048576;
	my $romMB = sprintf "%.2f", $totalRomSize/1048576;
	my $ramMB = sprintf "%.2f", $totalRamSize/1048576;
	print ",.text,.rodata,.data,.sdata,.sbss,.bss,.pdr(X),.stab(X),.stabstr(X),totalSize,romSize,ramSize,\n";
	print ",,,,,,,,,,", $totalSectionSize, ",", $totalRomSize, ",", $totalRamSize,", Bytes,\n";
	print ",,,,,,,,,,", $secKB, ",", $ramKB, ",", $romKB,", KBytes,\n";
	print ",,,,,,,,,,", $secMB, ",", $romMB, ",", $ramMB,", MBytes,\n";
}

sub by_size
{
	my ($sizA, $sizeB);

	$sizeA = $hash->{$a}->{"romSize"};
	$sizeB = $hash->{$b}->{"romSize"};
	#print $a, ":",  $sizeA, " ", $b, ":", $sizeB, "\n";
	$sizeB <=> $sizeA;
}

