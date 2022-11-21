#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <io.h>
#include <fcntl.h>
#include <ctype.h>

#define SAVE_FILE "CASTLE"

void find_path(const char* data, int cur_room, unsigned char* path_dir)
{
	// ��X�U�ж����ت��a���̵u���|��V //
	for (;;)
	{
		// ��X�U�趡���ت��a����V //
		bool updated = false;
		for (int i=1; i<=60; ++i)
		{
			if (path_dir[i] != 0xff) continue;	// �w�B�z�L //
			int pos = i*256+0x49;
			for (int j=0; j<5; ++j)
			{
				int room = data[pos+j];
				if ((room <= 0) || (room > 60)) continue;
				if (path_dir[room] == 0xff) continue;
				path_dir[i] = j;
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
		}
		int pos = cur_room*256+0x49;
		int next_room = data[pos+dir];
		if (next_room > 60) break;
		cur_room = next_room;
		if ((cur_room <= 0) || (cur_room > 60))
		{
			printf("bad cur room\n");
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
	for (int i=1; i<=60; ++i)
	{
		int pos = i*256+0x80;
		for (int j=0; j<8; ++j)
			if ((data[pos+j*0x10] == 0x30) && (data[pos+j*0x10+2] == item))
			{
				path_dir[i] = 0xfe;
				break;
			}
	}
	find_path(data,cur_room,path_dir);
}

//��X�f
//��J�Gdata = �ж����, cur_room = ���a�Ҧb�ж�
void find_out(const char* data, int cur_room)
{
	unsigned char path_dir[64];		// �U�ж����ت��a����V,0=��,1=�k,2=�W,3=�U,4=��,0xFE=�ت��a,0xFF=�|���w�q //
	memset(path_dir,0xff,sizeof(path_dir));
	// ���X�f�e���ж� //
	for (int i=1; i<=60; ++i)
	{
		int pos = i*256+0x49;
		for (int j=0; j<5; ++j)
			if ((unsigned char)data[pos+j] > 60)
			{
				path_dir[i] = j;
				break;
			}
	}
	find_path(data,cur_room,path_dir);
}

//��J�Gdata = �ж����, cur_room = ���a�Ҧb�ж�, has_plan = �O�_�w����p�e
void dump(const char* data, int cur_room, bool has_plan)
{
	if ((cur_room <= 0) || (cur_room > 60))
	{
		printf("bad cur room\n");
		return;
	}
	printf("�ثe�Ҧb�ж�: %d\n",cur_room);
	// �䪫�~: �@�ԭp�e //
	if (!has_plan)
	{
		printf("�@�ԭp�e: ");
		find_item(data,0x0F,cur_room);
	}
	// ��X�f //
	printf("�X�f: ");
	find_out(data,cur_room);
}

void main(void)
{
	// Ū����� //
	int file = open(SAVE_FILE,O_RDONLY | O_BINARY);
	if (file <= 0)
	{
		printf("�L�k�}���ɮ�: %s\n",SAVE_FILE);
		printf("�бN���{����b[�w�x�`��]�ؿ��U����\n");
	}
	else
	{
		char data[16384];
		read(file,data,61*256);
		close(file);
		int cur_room = data[0x40];
		bool has_plan = data[0x6c] != 0;
		dump(data,cur_room,has_plan);
	}
	system("pause");
}
