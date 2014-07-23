#!/c/Perl/bin/perl

use warnings;
use File::Copy;
use File::Find;
use Sys::Hostname;

sub logmsg
{
	my $msg = shift;
	my $date = localtime(time);
	return "$date: $msg\n";
}

sub file_lines_to_list
{
	my	$dat_file = $_[0];
	my	@list1 = ();
	open (DATA_FILE, $dat_file) or die "ERROR: can't open file $dat_file\n$!\n";

	while ($record = <DATA_FILE>) {
		chomp($record);
	   	push(@list1,$record);
	}

   close(DATA_FILE);
   return @list1; 
}

sub add_copyrights
{
	my ($root_dir, $header_file_name) = @_;
	
	system "touch copyrights_files_$$.txt";
	
    #system "bash -c \"/usr/bin/find $root_dir -type f -name \\\"*.asm\\\" > copyrights_files_$$.txt\"";	
    system "bash -c \"/usr/bin/find $root_dir -type f -name \\\"*.S\\\" >> copyrights_files_$$.txt\"";	
	system "bash -c \"/usr/bin/find $root_dir -type f -name \\\"*.c\\\" >> copyrights_files_$$.txt\"";
	system "bash -c \"/usr/bin/find $root_dir -type f -name \\\"*.h\\\" >> copyrights_files_$$.txt\"";
	system "bash -c \"/usr/bin/find $root_dir -type f -name \\\"*.cc\\\" >> copyrights_files_$$.txt\"";
	system "bash -c \"/usr/bin/find $root_dir -type f -name \\\"*.cpp\\\" >> copyrights_files_$$.txt\"";
	system "bash -c \"/usr/bin/find $root_dir -type f -name \\\"*.hpp\\\" >> copyrights_files_$$.txt\"";

	my @list_h = file_lines_to_list("copyrights_files_$$.txt");
	system "rm -f copyrights_files_$$.txt";

	my $num_of_elem = @list_h;
	if ($num_of_elem == 0) {
		print logmsg "ERROR adding copyrights!!! number of ellements is $num_of_elem";
	}

	my $header = `cat $header_file_name`;
	
	foreach my $file1 (@list_h)
	{
		my $num = `grep -ic "Copyright" $file1`;
		chomp($num);
		
		if($num > 0) {
			print logmsg "copyrights exists in $file1";
		}	

        #skip files which already have a copyright
		next if($num > 0);
		
		############################################################################# TODO - Also for .S files
		#add ; for comments in case of assembly file                                  
		my $file_header;
        if ($file1=~m/\.asm(\r)?(\n)?$/) {
            $file_header = ';'.$header;
            $file_header =~s/\n/\n\;/g;
            $file_header =~s/\;$//;
        } 
        else {
            $file_header = $header;
        }         		
               
		#$file_header =~ s/file_name/$file_name/;
		open(INFO, ">$file1._temp") or die "can't open file $file1._temp\n$!\n";
		print INFO $file_header;
		close(INFO);	
		
		system "cat $file1 >> $file1._temp";
		system "rm $file1";
		system "mv $file1.\_temp $file1";
		#system "dos2unix $file1"; ######################################################### TODO - Don't think this is needed
		print logmsg "copyrights were added to $file1";
	}
}	

# START here
my $argc = @ARGV;
if ($argc < 1 || $argc  > 3)
{
	print ("incorrect arguments\n");
	print ("usage: add_copyrights <path to root dir> <copyright_text>\n");
	exit(1);
}

my $root_dir = $ARGV[0];
my $copyright_text =  $ARGV[1];

add_copyrights($root_dir, $copyright_text);













