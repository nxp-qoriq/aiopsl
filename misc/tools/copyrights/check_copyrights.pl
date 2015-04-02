#!/cygdrive/c/Perl/bin/perl

use strict;
use warnings;
use File::Find;

my $copyright_text;

my $argc = @ARGV;
if ($argc < 1)
{
	print ("Incorrect arguments!!\n");
	&usage;
	exit(1);
}

parse_argv();

our $start_dir;
our @files= " ";
our $path;
#our $errors_file= "copyrights_errors.txt";
our $error_flg=0;

$start_dir =~ s/\\/\//g;

system "bash -c \"/usr/bin/find $start_dir -type f -name \\\"*.asm\\\" > copyright_files.txt\"";	
system "bash -c \"/usr/bin/find $start_dir -type f -name \\\"*.S\\\" > copyright_files.txt\"";	
system "bash -c \"/usr/bin/find $start_dir -type f -name \\\"*.c\\\" >> copyright_files.txt\"";
system "bash -c \"/usr/bin/find $start_dir -type f -name \\\"*.h\\\" >> copyright_files.txt\"";
# system "bash -c \"/usr/bin/find $start_dir -type f -name \\\"*.cc\\\" >> copyright_files.txt\"";
# system "bash -c \"/usr/bin/find $start_dir -type f -name \\\"*.cpp\\\" >> copyright_files.txt\"";
# system "bash -c \"/usr/bin/find $start_dir -type f -name \\\"*.hpp\\\" >> copyright_files.txt\"";

@files = file_lines_to_list("copyright_files.txt");

if (! @files) { 
	die "Error: There are no files to read!! check dir argeument.\n";}

#open(ERROR, ">$errors_file");

#print ERROR "Copyright is missing for these files:\n-------------------------------------------------------------------------------------------------------\n\n";
	
foreach $path(@files) {

	#if ($path =~ /tools\//){ print LOG "Ignoring $path - Path contains /tools/\n";next;}
	if ( (`grep -ic   'Copyright 2014-2015 Freescale Semiconductor, Inc.' $path`) >0 ){
		if ( (`grep -ic   'Redistribution and use in source and binary forms, with or without' $path`) >0 ){
			#print "Copyright is ok in $path\n";
		}
		else{ 
			$error_flg=1;
			print "Copyright is Corrupted in $path\n";
		}
	}
	else { 	
		#check if flib version
		if( (`grep -ic   'Copyright 2013-2015 Freescale Semiconductor Inc.' $path`) >0 ) {
			if ( (`grep -ic   'ALTERNATIVELY, this software may be distributed under the terms of the' $path`) >0 ){
				#print "Copyright is ok in $path\n";
			}
			else{ 
				$error_flg=1;
				print "Copyright is Corrupted in $path\n";
			}
		}
		else {
			$error_flg=1;
			print "Copyright is missing in $path\n";
			#print ERROR "$path  \n";
		}
	}
	
}
if ($error_flg == 1){
	print "Copyright verification failed\n";
	exit 1;
}else {
	print "Copyright verification passed\n";
	exit 0;
}

#close ERROR;
#if ($error_flg == 0){ # There are no errors.
#	system ("rm $errors_file");
#}

system ("rm copyright_files.txt");

# -----------------------------------------------------------------------------------------------------------	

sub parse_argv
{

    while (@ARGV)
    {
	    my $arg = shift(@ARGV);

    	PARSEIT:
        {
		 $arg eq '-dir' && do { $start_dir = shift(@ARGV); last PARSEIT; };
		 $arg eq '-text' && do { $copyright_text = shift(@ARGV); last PARSEIT; };
		    # For unknown switch:
    	    print "Unknown Argument- $arg\n";
    	    &usage;
    	    exit 1;
		}
		
	}

	
# -----------------------------------------------------------------------------------------------------------	

sub usage
{
    print "Usage: perl check_copyrights.pl -dir <dir> -text <copyright file>
-dir	This argument is the root directory for our search- it should contain all c,h,asm files after checkout.
-text   This is the path to the file containing the correct copyright message.
";
}

# -----------------------------------------------------------------------------------------------------------	

}
sub file_lines_to_list
{
	our	$dat_file = $_[0];
	our	@list1 = ();
	our $record;
	open (DATA_FILE, $dat_file);

	while ($record = <DATA_FILE>) {
		chomp($record);
	   	push(@list1,$record);
	}

   close(DATA_FILE);
   return @list1; 
}
