BEGIN {} 

# http://www.avrfreaks.net/index.php?name=PNphpBB2&file=printview&t=33291&start=0

# add a record end marker 
END { print ":00000001FF" } 

# delete the record end marker 
/^:00000001FF/{ $0 = ""} 

# default action: output all non-blank lines 
{ if ($0 != ""){ print } } 