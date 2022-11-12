#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <io.h>
#include <fcntl.h>
#include <ctype.h>

#define SAVE_FILE "CASTLE"

int get_current_room(char* data)
{
	return (unsigned char)data[0x40];
}

void dump_path(char* data)
{
	unsigned char path_dir[64];		// �U�ж����X�f����V,0=��,1=�k,2=�W,3=�U,4=��,0xFF=�|���w�q //
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
	// ��X�U�ж����X�f���̵u���|��V //
	for (;;)
	{
		// ��X�U�趡���X�f����V //
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
	// ���o�ثe���a�Ҧb���ж��s�� //
	int cur_room = get_current_room(data);
	if ((cur_room <= 0) || (cur_room > 60))
	{
		printf("bad cur room value\n");
		return;
	}
	printf("�ثe�Ҧb�ж��G#%d\n",cur_room);
	printf("�k���V�G");
	// ��ܤ�V //
	for (;;)
	{
		int dir = path_dir[cur_room];
		if (dir == 0xff)
		{
			printf("bad path dir (no exit)\n");
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
			printf("bad cur room value\n");
			return;
		}
	}
	printf("\n");
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
		read(file,data,63*256);
		close(file);
		dump_path(data);
	}
	system("pause");
}
