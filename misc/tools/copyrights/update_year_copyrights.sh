git grep -l Copyright | while read p
do
    sed -i 's/Copyright 2014 Freescale/Copyright 2014-2015 Freescale/' ${p}
	echo "Updated: ${p}"
done
