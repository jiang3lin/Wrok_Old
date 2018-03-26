#!/usr/bin/perl

open(FILE, $ARGV[0]) || die "Cannot open $ARGV[0]\n";

%hash = ();

while(<FILE>)
{
	if (/^\s*(\..+?)\s*0x\w{8}\s*(0x.+?)\s+?(.+?\.a)/)
	{
		$section = $1;
		$size = hex($2);
		$lib = $3;
		$lib =~ s/\\/\//g;
		$lib =~ s/.+\/(.+\.a)/$1/;
		#print "$section, $size, $lib\n";
		$hash->{$lib}->{$section} += $size;
	}
}
close(FILE);


# Save Total Size first
foreach $libs (keys (%{$hash}))
{
	my ($size, $binsize, $sections);
	$size = 0;
	$binsize = 0;

	foreach $sections (keys (%{$hash->{$libs}}))
	{
		$size += $hash->{$libs}->{$sections};
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
			$binsize += $hash->{$libs}->{$sections};
		}
	}
	$hash->{$libs}->{"size"} = $size;
	$hash->{$libs}->{"binsize"} = $binsize;
}


foreach $libs (sort by_size keys (%{$hash}))
{
	my ($sections);

	print $libs, "\n";
	foreach $sections (sort {$a cmp $b} keys (%{$hash->{$libs}}))
	{
		if (($sections cmp "size") && ($sections cmp "binsize"))
		{
			$empty = 20 - length($sections);
			print "\t$sections";
		   	print " " x $empty, $hash->{$libs}->{$sections}, "\n";
		}
	}
	print "-" x 34, "\n";
	print "\tBinSize: ", " " x 11, $hash->{$libs}->{"binsize"}, "\n";
	print "\tTotal: ", " " x 13, $hash->{$libs}->{"size"}, "\n\n";
}


$totalSize = 0;
foreach $libs (keys (%{$hash}))
{
	my ($sections);

	foreach $sections (keys (%{$hash->{$libs}}))
	{
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
			$totalSize += $hash->{$libs}->{$sections};
		}
	}
}
print "TotalSize: ", $totalSize, "\n";

sub by_size
{
	my ($sizA, $sizeB);

	$sizeA = $hash->{$a}->{"binsize"};
	$sizeB = $hash->{$b}->{"binsize"};
	#print $a, ":",  $sizeA, " ", $b, ":", $sizeB, "\n";
	$sizeB <=> $sizeA;
}

