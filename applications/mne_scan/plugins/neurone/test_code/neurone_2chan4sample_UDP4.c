/*
* @file		NeurOne_2chan4sample_UDP4.c
* @author	Nils Schneider <schneidergarces@gmail.com>
*
* @date     	November 2017	
*
* @description	reading 2 channels with 4 sample points per packet each from ethernet port number 5000 
*/


#include <stdio.h>
#include <winsock2.h>
#include <stdint.h>
#include "Sample32bit.h" // converting UInt8 byte arrays into raw sample points

#pragma comment(lib,"ws2_32.lib") //Winsock Library

//int Sample32bit(unsigned char *byteArray);


int main()
{ 
	WORD wVersionRequested;
	WSADATA wsaData;
	int wsaerr;
	//unsigned int result;	

	// Using MAKEWORD macro, Winsock version request 2.2
	wVersionRequested = MAKEWORD(2, 2);
	wsaerr = WSAStartup(wVersionRequested, &wsaData);

	if (wsaerr != 0)
	{
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.*/
		printf("The Winsock dll not found!\n");
		return 0;
	}


	
	
	//////////Create a socket////////////////////////
	//Create a SOCKET object called m_socket.
	SOCKET m_socket;

	// Call the socket function and return its value to the m_socket variable.
	// For this application, use the Internet address family, streaming sockets, and
	// the TCP/IP protocol.
	// using AF_INET family, TCP socket type and protocol of the AF_INET - IPv4
	m_socket = socket(AF_INET, SOCK_DGRAM, 0);

	// Check for errors to ensure that the socket is a valid socket.
	if (m_socket == INVALID_SOCKET)
	{
		printf("Error at socket(): %ld\n", WSAGetLastError());
		WSACleanup();
		return 0;
	}
	

	////////////////bind()//////////////////////////////
	// Create a sockaddr_in object and set its values.

	struct sockaddr_in service;

	// AF_INET is the Internet address family.
	service.sin_family = AF_INET;
	// INADDE_ANY listen all.
	service.sin_addr.s_addr = INADDR_ANY;
	// 50000 is the port number to which the socket will be bound.
	service.sin_port = htons(50000);

	// Call the bind function, passing the created socket and the sockaddr_in structure as parameters.
	// Check for general errors.

	if (bind(m_socket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
	{
		printf("bind() failed: %ld.\n", WSAGetLastError());
		closesocket(m_socket);
		return 0;
	}
	

	////////////// make space for data /////////////////

	// number of channels
	int n_chan = 2;
	// number of samples per channel
	int n_sample = 4;	
	// package size, first 28 for header + 3*n_sample*n_chan for channels: 52 bytes for 2 channels
	int pckSize = 28 + 3 * n_sample * n_chan;
	// size of buffer, set to maximum IP4 packet size, 64kb
	char bufSize = 64000;


	int bytesRecv;
	unsigned char *recvbuf=0;
	
	recvbuf = malloc(sizeof(bufSize)); // allocate space for buffer
	
	// space for UInt8 byte arrays, one array per channel with 3 UInt8's for each sample point
	unsigned char *arr1=0;	
	arr1 = malloc(sizeof(char[3]));

	unsigned char *arr2 = 0;
	arr2 = malloc(sizeof(char[3]));
	
	// space for final raw sample points 
	int result1[4] = {0,0,0,0};	
	int result2[4] = {0,0,0,0};

	printf("Ready, waiting for data\n");


	////////////  test read package, needs to be done once before reading real data /////////////////////

	bytesRecv = recv(m_socket, recvbuf, pckSize, 0); 
	if (bytesRecv <0)
	{
	printf("recv failed: %d\n", WSAGetLastError());
	}

	printf(" %ld.\n", bytesRecv);
	printf("%s\n",recvbuf);
	
	
	//  test read package for channel number and n_sample, not used yet
	bytesRecv = recv(m_socket, recvbuf, pckSize, 0);
	if (bytesRecv <0)
	{
	printf("recv failed: %d\n", WSAGetLastError());
	}

	printf("ID: %u; Channels: %u;  n_sample: %u;\n", recvbuf[0], recvbuf[9], recvbuf[11]);


	/////////// start reading packages and converting UInt8 to raw data sample points ////////////////////

	do {
		
	
		bytesRecv = recv(m_socket, recvbuf, pckSize, 0);
		if (bytesRecv <0)
		{
		printf("recv failed: %d\n", WSAGetLastError());
		}
	int i;

	for ( i = 0; i<n_sample; i++) {
		arr1[0]=recvbuf[28+i*3];
		arr1[1]=recvbuf[29+i*3];
		arr1[2]=recvbuf[30+i*3];

		arr2[0] = recvbuf[31 + i * 3];
		arr2[1] = recvbuf[32 + i * 3];
		arr2[2] = recvbuf[33 + i * 3];

		(int)result1[i] = Sample32bit(arr1);

		(int)result2[i] = Sample32bit(arr2);

		arr1 = malloc(sizeof(char[4]));
		arr2 = malloc(sizeof(char[4]));
	
		}	 

		printf("Channel1: %ld. %d %d %d %d\n", bytesRecv, result1[0], result1[1], result1[2], result1[3]);	
		printf("Channel2: %ld. %d %d %d %d\n", bytesRecv, result2[0], result2[1], result2[2], result2[3]);

		
	} while (recvbuf[0] < 4);
	
	
	
	WSACleanup();
	return 0;
}