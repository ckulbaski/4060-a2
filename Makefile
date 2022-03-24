CC = g++ 
CFLAGS = -Wall -Wextra -Werror 

main: A2Q1_Kulbaski_C A2Q2_Kulbaski_C

A2Q1_Kulbaski_C: lzw.cpp
	$(CC) $(CFLAGS) lzw.cpp -o A2Q1_Kulbaski_C   
A2Q2_Kulbaski_C: bwt.cpp
	$(CC) $(CFLAGS) bwt.cpp -o A2Q2_Kulbaski_C   

clean: 
	rm -f A2Q1_Kulbaski_C A2Q2_Kulbaski_C *.lzw *.dlzw *.bwt *.dbwt
clean-out: 
	rm -f *.lzw *.dlzw *.bwt *.dbwt

