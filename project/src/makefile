
project: main.o DNS.o DNS1.o main1.o
	  g++ main.o DNS.o -o project
	  g++ main1.o DNS1.o -o project1

main1.o:
	  g++ -c main1.cpp -o main1.o

DNS1.o:
	  g++ -c DNS1.cpp -o DNS1.o

main.o:
	  g++ -c main.cpp -o main.o

DNS.o:
	  g++ -c DNS.cpp -o DNS.o

clean:
	  rm *.o project project1
