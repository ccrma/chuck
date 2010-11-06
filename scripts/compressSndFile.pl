use Data::Dumper;
my %seen;
my @source;

my $output = $ARGV[0];

my @fileList = `ls *.c *.h`;
foreach $_ (@fileList)
{
	chomp;
	if( $_ =~ /$output/ || $_ =~ /test/ ) {}
	elsif( $_ =~ /\.h/ ) { $seen{$_} = 0; }
	elsif( $_ =~ /\.c/ ) {  push @source, $_ }
}

close DOTH;
close DOTC;
open DOTH, ">" . $output . ".h" or die;
open DOTC, ">" . $output . ".c" or die;
my @list;
$seen{'config.h'} = 1;
push @list, "config.h";
$seen{'sndfile.h'} = 1;
push @list, "sndfile.h";
$seen{'common.h'} = 1;
push @list, "common.h";
$seen{'G72x.h'} = 1;
push @list, "G72x.h";

sub depend
{
	my $file = shift;
	my $handle;
	open $handle, "<$file";
	while(<$handle>)
	{
		if( my $include = /#include[ \t]*[<"]([\w\d]+\.h)[>"]/ )
		{
			if( defined( $seen{$include} && $seen{$include} == 0 ) )
			{
				depend( $include );
			}
		}
	}
	close $handle;
	$seen{$file} = 1;
	push @list, $file;
}

foreach my $key ( keys %seen )
{
	if( $seen{$key} == 0 ) { depend($key); }
}

print DOTH '#include <stdio.h>' . "\n\n";
print DOTH '#include <stdlib.h>' . "\n\n";
foreach my $file (@list)
{
	open F, "<$file";
	while(<F>)
	{
		if( $_ !~ /#include[ \t]*[<"](\w+\.h)[>"]/ )
		{
			print DOTH $_;
		}
	}
	close F;
}





close FILE;

print DOTC '#include ' . "\"$output.h\"\n";
print DOTC '#include <stdio.h>' . "\n\n";
print DOTC '#include <stdlib.h>' . "\n\n";
foreach my $file (@source)
{
	open FILE, $file;
	$file =~ s/\.[ch]//g;
	while( my $line = <FILE> )
	{
		if( $line !~ /#include[ \t]*("[\w\d]+\.[ch]")|<CoreServices.h>/ )
	       	{
			$line =~ s/(f2d_array)/$file$1/g;
			$line =~ s/(d2f_array)/$file$1/g;
			$line =~ s/(_\w+tab)/$file$1/g;
			$line =~ s/(STEP)/$file$1/g;
			$line =~ s/(HAVE_[^D]\w+)/$file$1/g;
		       	print DOTC $line; 
		}
	}
	close FILE;
}



close DOTH;
close DOTC;



