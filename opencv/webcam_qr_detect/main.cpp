/* Created by JUB, KMJ */
/* This file contains QR detect code and socket communication */
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/imgcodecs.hpp>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <signal.h>

#define BUF_SIZE 100
#define NAME_SIZE 20
#define ARR_CNT 5

using namespace std;
using namespace cv;

void* send_msg(void* arg);
void* recv_msg(void* arg);
void error_handling(char* msg);

char name[NAME_SIZE] = "[Default]";
char msg[BUF_SIZE];
char sample_msg[BUF_SIZE]={0};
void display(Mat& im, Mat& bbox);

int main(int argc, char* argv[])
{
	int sock;
	struct sockaddr_in serv_addr;
	pthread_t snd_thread, rcv_thread;
	void* thread_return;

	if (argc != 4) {
		printf("Usage : %s <IP> <port> <name>\n", argv[0]);
		exit(1);
	}

	sprintf(name, "%s", argv[3]);

	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock == -1)
		error_handling("socket() error");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));

	if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
		error_handling("connect() error");

	sprintf(msg, "[%s:PASSWD]", name);
	write(sock, msg, strlen(msg));
	pthread_create(&rcv_thread, NULL, recv_msg, (void*)&sock);
	pthread_create(&snd_thread, NULL, send_msg, (void*)&sock);

	pthread_join(snd_thread, &thread_return);
	pthread_join(rcv_thread, &thread_return);

	close(sock);
	return 0;
}
void display(Mat& im, Mat& bbox)
{
	int n = bbox.rows;
	for (int i = 0; i < n; i++)
	{
		line(im, Point2i(bbox.at<float>(i, 0), bbox.at<float>(i, 1)),
		Point2i(bbox.at<float>((i + 1) % n, 0), bbox.at<float>((i + 1) % n, 1)),
														Scalar(255, 0, 0), 3);
	}
	imshow("Result", im);
}

void* send_msg(void* arg)
{
	int* sock = (int*)arg;
	int str_len;
	int ret;
	fd_set initset, newset;
	struct timeval tv;
	char name_msg[NAME_SIZE + BUF_SIZE + 2];

	FD_ZERO(&initset);
	FD_SET(STDIN_FILENO, &initset);

	fputs("Input a message! [ID]msg (Default ID:ALLMSG)\n", stdout);
	while (1) {
		memset(msg, 0, sizeof(msg));
		name_msg[0] = '\0';
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		newset = initset;
		ret = select(STDIN_FILENO + 1, &newset, NULL, NULL, &tv);
		if (FD_ISSET(STDIN_FILENO, &newset))
		{
			fgets(msg, BUF_SIZE, stdin);
			if (!strncmp(msg, "quit\n", 5)) {
				*sock = -1;
				return NULL;
			}
			else if (msg[0] != '[')
			{
				strcat(name_msg, "[ALLMSG]");
				strcat(name_msg, msg);
			}
			else
				strcpy(name_msg, msg);
			if (write(*sock, name_msg, strlen(name_msg)) <= 0)
			{
				*sock = -1;
				return NULL;
			}
		}
		if (ret == 0)
		{
			if (*sock == -1)
				return NULL;
		}
	}
}

void* recv_msg(void* arg)
{
	int* sock = (int*)arg;
	int i;
	char* pToken;
	char* pArray[ARR_CNT] = { 0 };

	char name_msg[NAME_SIZE + BUF_SIZE + 1];
	int str_len;

	while (1) {
		memset(name_msg, 0x0, sizeof(name_msg));
		memset(sample_msg, 0x0, sizeof(sample_msg));
		str_len = read(*sock, name_msg, NAME_SIZE + BUF_SIZE);
		if (str_len <= 0)
		{
			*sock = -1;
			return NULL;
		}
		name_msg[str_len] = 0;
		fputs(name_msg, stdout);
		printf("capture start!\n");
		Mat frame;
		VideoCapture cap(0, CAP_V4L2); // camera link
		if (!cap.isOpened())
		{
			cerr << "can't open camera." << endl;
			exit(1);
		}
		double fps = cap.get(CAP_PROP_FPS);
		int width = cap.get(CAP_PROP_FRAME_WIDTH); // webcam width
		int height = cap.get(CAP_PROP_FRAME_HEIGHT); // webcam height
		int fourcc = VideoWriter::fourcc('X', 'V', 'I', 'D'); // set codec

		QRCodeDetector qrDecoder;

		while (true) // cam read
		{
			cap.read(frame);
			if (frame.empty())
			{
				cerr << "capture failed." << endl;
				break;
			}
			Mat bbox, rectifiedImage;
			//QR detect ON
			string data = qrDecoder.detectAndDecode(frame, bbox, rectifiedImage);
            char product[2];

			if (data.length() > 0)
			{
				cout << "Decoded Data:" << data << endl;
				strcpy(product,data.c_str());
                sprintf(sample_msg, "[ARD]DEV@Product@%s\n", product);
                write(*sock, sample_msg, strlen(sample_msg));
				break;
			}
			else {}
			imshow("QRCodeScanner", frame);
			int wait = int(1.0 / fps * 1000); // camera delay
			int key = waitKey(1);
			if (key == 27) // esc key to quit program
			{
				break;
			}
		}
	}
}

void error_handling(char* msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
