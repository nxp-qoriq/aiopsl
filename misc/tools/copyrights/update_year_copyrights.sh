git grep -l Copyright | while read p
do
    sed -i 's/Copyright 2014 Freescale/Copyright 2014-2015 Freescale/' ${p}
	sed -i 's/Copyright 2013-2014 Freescale Semiconductor Inc/Copyright 2014-2015 Freescale Semiconductor Inc/' ${p}
	
	echo "Updated flb: ${p}"
done
