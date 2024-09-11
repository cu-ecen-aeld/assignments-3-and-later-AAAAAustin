# Austin Spaulding 
# writer.sh script
# assignment 1

writefile=$1
writestr=$2
partialpath=${writefile%/*} #extracting path w/o file as argument for mkdir command later

if [ $# -gt 1 ];

	then
		mkdir -p $partialpath
		touch $writefile 
		echo $writestr > $writefile
	else
	        echo "writer.sh incorrect number of parameters"
		exit 1
fi

