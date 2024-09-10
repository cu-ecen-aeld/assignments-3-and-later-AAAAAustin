# Austin Spaulding
# finder.sh
# assignment 1

filesdir=$1
searchstr=$2

if ! [ -d $filesdir ] ;
	then
		echo "finder.sh directory does not exist"
		exit 1
	else
		if [ $# -lt 2 ];

			then
				echo "finder.sh parameters are in incorrect format"
                                exit 1
			else
				num_of_files=$( find $filesdir -type f | wc -l ) #printing out all files in dir as column and using wc -l to count lines
				num_of_matches=$( grep -r $searchstr $filesdir | wc -l ) #printing out all matching lines as column using grep -- using wc -l to count lines
				echo "The number of files are $num_of_files and the number of matching lines are $num_of_matches"

		fi
fi
 
