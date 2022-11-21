#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <io.h>
#include <fcntl.h>
#include <ctype.h>

#define SAVE_FILE "GAME.DAT"

const char* item_name[] = 
   {"�L",
    "�l�u",
    "����",
    "�q����",
    "���ı�",
    "��U��",
    "���N�~",
    "�_��",
    "�_��",
    "��O",
    "ĵ���t��",
    "�P��",
    "��Ͻc",
    "�}��u��",
    "�q�D",
    "���u"};

void find_path(const char* data, int cur_room, unsigned char* path_dir)
{
	// ��X�U�ж����ت��a�̵u���|��V //
	for (;;)
	{
		// ��X�U�趡���ت��a����V //
		bool updated = false;
		for (int i=1; i<=63; ++i)
		{
			if (path_dir[i] != 0xff) continue;	// �w�B�z�L //
			int pos = (i-1)*256+0x49;
			for (int j=0; j<5; ++j)
			{
				int room = data[pos+j];
				if ((room <= 0) || (room > 63)) continue;
				if (path_dir[room] == 0xff) continue;
				path_dir[i] = j;
				updated = true;
				break;
			}
			pos = (i-1)*256+0x80;
			for (int j=0; j<8; ++j)
				if ((data[pos+j*0x10] == 0x20) && (data[pos+j*0x10+2] == 0xe))
				{
					int room = data[pos+j*0x10+10];
					if ((room <= 0) || (room > 63)) break;
					if (path_dir[room] == 0xff) break;
					path_dir[i] = 5;
					updated = true;
					break;
				}
		}
		if (!updated) break;
	}
	// ��ܤ�V //
	for (;;)
	{
		int dir = path_dir[cur_room];
		// ��ܸөж����o�d���~ //
		/*
		int pos = (cur_room-1)*256+0x80;
		bool has_data = false;
		for (int j=0; j<8; ++j)
			if (data[pos+j*0x10] == 0x20)
			{
				int item = data[pos+j*0x10+2];
				if (item == 0) continue;
				if (item > 0xf)
				{
					printf("bad item");
					return;
				}
				if (has_data) printf(",");
				else printf("(");
				has_data = true;
				printf("%s",item_name[item]);
			}
		if (has_data) printf(")");
		*/
		if (dir == 0xfe) break;
		if (dir == 0xff)
		{
			printf("�L\n");
			return;
		}
		switch(dir)
		{
		case 0: printf("��"); break;
		case 1: printf("�k"); break;
		case 2: printf("�W"); break;
		case 3: printf("�U"); break;
		case 4: printf("��"); break;
		case 5: printf("�o"); break;
		}
		if (dir == 5)
		{
			int pos = (cur_room-1)*256+0x80;
			for (int j=0; j<8; ++j)
				if ((data[pos+j*0x10] == 0x20) && (data[pos+j*0x10+2] == 0xe))
				{
					cur_room = data[pos+j*0x10+10];
					break;
				}
		}
		else
		{
			int pos = (cur_room-1)*256+0x49;
			cur_room  = data[pos+dir];
		}
		if ((cur_room <= 0) || (cur_room > 63))
		{
			printf("bad room\n");
			return;
		}
	}
	printf("\n");
}

//�䪫�~
//��J�Gdata = �ж����, item = ���~�s��, cur_room = ���a�Ҧb�ж�
void find_item(const char* data, int item, int cur_room)
{
	unsigned char path_dir[64];		// �U�ж����ت��a����V,0=��,1=�k,2=�W,3=�U,4=��,5=�o,0xFE=�ت��a,0xFF=�|���w�q //
	memset(path_dir,0xff,sizeof(path_dir));
	// ��쪫�~�Ҧb���ж� //
	for (int i=1; i<=63; ++i)
	{
		int pos = (i-1)*256+0x80;
		for (int j=0; j<8; ++j)
			if ((data[pos+j*0x10] == 0x20) && (data[pos+j*0x10+2] == item))
			{
				path_dir[i] = 0xfe;
				break;
			}
	}
	find_path(data,cur_room,path_dir);
}

//��|ĳ��
//��J�Gdata = �ж����, cur_room = ���a�Ҧb�ж�
void find_room(const char* data, int cur_room)
{
	unsigned char path_dir[64];		// �U�ж����ت��a����V,0=��,1=�k,2=�W,3=�U,4=��,5=�o,0xFE=�ت��a,0xFF=�|���w�q //
	memset(path_dir,0xff,sizeof(path_dir));
	// ��쪫�~�Ҧb���ж� //
	for (int i=1; i<=63; ++i)
	{
		int pos = (i-1)*256+0x80;
		for (int j=0; j<8; ++j)
			if (data[pos+j*0x10] == 0x60)
			{
				path_dir[i] = 0xfe;
				break;
			}
	}
	find_path(data,cur_room,path_dir);
}

//��^�{
//��J�Gdata = �ж����, cur_room = ���a�Ҧb�ж�
void find_back_path(const char* data, int cur_room)
{
	unsigned char path_dir[64];		// �U�ж����ت��a����V,0=��,1=�k,2=�W,3=�U,4=��,5=�o,0xFE=�ت��a,0xFF=�|���w�q //
	memset(path_dir,0xff,sizeof(path_dir));
	path_dir[1] = 0xfe;	// �^��ж�#1 //
	find_path(data,cur_room,path_dir);
}

//��J�Gdata = �ж����, cur_room = ���a�Ҧb�ж�, has_boom = �O�_�w�����u
void dump(const char* data, int cur_room, bool has_boom)
{
	if ((cur_room <= 0) || (cur_room > 63))
	{
		printf("bad cur room\n");
		return;
	}
	printf("�ثe�Ҧb�ж�: %d\n",cur_room);
	// �䪫�~: ĵ���t�� //
	//printf("ĵ���t��: ");
	//find_item(data,0x0A,cur_room);
	// �䪫�~: �w�ɬ��u //
	if (!has_boom)
	{
		printf("���u: ");
		find_item(data,0x0F,cur_room);
	}
	// ��|ĳ�� //
	printf("�|ĳ��: ");
	find_room(data,cur_room);
	// ��^�{ //
	if (cur_room > 1)
	{
		printf("�^�{: ");
		find_back_path(data,cur_room);
	}
}

void main(void)
{
	// Ū����� //
	int file = open(SAVE_FILE,O_RDONLY | O_BINARY);
	if (file <= 0)
	{
		printf("�L�k�}���ɮ�: %s\n",SAVE_FILE);
		printf("�бN���{����b[�w�x�`��2]�ؿ��U����\n");
	}
	else
	{
		char data[16384];
		int start_pos = 0x14400;
		for (int i=0; i<4; ++i)
		{
			int pos = 0x14400 + i*0x2400;
			lseek(file,pos,SEEK_SET);
			read(file,data+i*0x1000,0x1000);
		}
		close(file);
		int cur_room = data[0x22];
		bool has_boom = data[4] != 0;
		dump(data+0xfc,cur_room,has_boom);
	}
	system("pause");
}
