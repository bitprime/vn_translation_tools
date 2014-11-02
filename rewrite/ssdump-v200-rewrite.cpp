#include <vector>
using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include "structs.h"

int nameArrayLen = 217;
wchar_t *nameArray[] = {
	L"？？？",  	L"???", 
	L"？？",  	L"??", 
	L"？",  	L"?", 
	L"おっさん",  	L"Old Man", 
	L"おばあちゃん",  	L"Old Lady", 
	L"ぎる",  	L"Gil", 
	L"しまこ",  	L"Shimako", 
	L"ちはや",  	L"Chihaya", 
	L"ともこ",  	L"Tomoko", 
	L"ねこ",  	L"Neko", 
	L"ぱに",  	L"Pani", 
	L"りぼん",  	L"Ribbon", 
	L"イタチ",  	L"Itachi", 
	L"オバチャン",  	L"Old Lady", 
	L"クラスメイト",  	L"Classmate", 
	L"ケリー",  	L"Kelly", 
	L"ツチノコ女",  	L"Clumsy Woman", 
	L"テレビ",  	L"TV", 
	L"ハルカ",  	L"Haruka", 
	L"ミドウ",  	L"Midou", 
	L"ルチア",  	L"Lucia", 
	L"ローブの男",  	L"Robed Man", 
	L"安西",  	L"Ansai", 
	L"井上",  	L"Inoue", 
	L"一同",  	L"Everyone", 
	L"一年女子",  	L"First-Year Girl", 
	L"一年生",  	L"First-Year", 
	L"一年生男子",  	L"First-Year Boy", 
	L"一年男子",  	L"First-Year Boy", 
	L"一年男子（派手）",  	L"First-Year Boy (flashy)", 
	L"一年鈴木Ａ",  	L"First-Year Suzuki A", 
	L"一年鈴木Ｂ",  	L"First-Year Suzuki B", 
	L"英語文",  	L"English Text", 
	L"花子？",  	L"Hanako?", 
	L"吉野",  	L"Yoshino", 
	L"救急隊員",  	L"救急隊員", 
	L"教師",  	L"Teacher", 
	L"警備員Ａ",  	L"Guard A", 
	L"警備員Ｂ",  	L"Guard B", 
	L"犬",  	L"Dog", 
	L"研究員",  	L"Researcher", 
	L"鍵",  	L"Key", 
	L"鍵の少女",  	L"Key", 
	L"元会長",  	L"Ex-president", 
	L"瑚太朗・小鳥",  	L"Kotarou & Kotori", 
	L"瑚太朗・鳳",  	L"Kotarou & Ootori", 
	L"瑚太朗",  	L"Kotarou", 
	L"江坂",  	L"Esaka", 
	L"江頭",  	L"Egashira", 
	L"今時の男",  	L"Contemporary Man", 
	L"今風の男",  	L"Contemporary Man", 
	L"咲夜",  	L"Sakuya", 
	L"三年女子",  	L"Third-Year Girl", 
	L"三年生女子",  	L"Third-Year Girl", 
	L"三年生男子",  	L"Third-Year Boy", 
	L"三年男子",  	L"Third-Year Boy", 
	L"子供",  	L"Child", 
	L"自転車屋",  	L"Bicycle Shop", 
	L"朱音",  	L"Akane", 
	L"女",  	L"Woman", 
	L"女の子",  	L"Girl", 
	L"女の声",  	L"Female Voice", 
	L"女子生徒",  	L"Schoolgirl", 
	L"女子生徒Ａ",  	L"Schoolgirl A", 
	L"女子生徒Ｂ",  	L"Schoolgirl B", 
	L"女子生徒Ｃ",  	L"Schoolgirl C", 
	L"女性",  	L"Woman", 
	L"女性の声",  	L"Woman's Voice", 
	L"女生徒",  	L"Schoolgirl", 
	L"小学生",  	L"Elementary Schoolkid", 
	L"小鳥",  	L"Kotori", 
	L"少女",  	L"Girl", 
	L"少年",  	L"Boy", 
	L"職員",  	L"Staff", 
	L"紳士",  	L"Gentleman", 
	L"須々木",  	L"Suzuki", 
	L"生徒",  	L"Student", 
	L"生徒たち",  	L"Students", 
	L"声",  	L"Voice", 
	L"静流",  	L"Shizuru", 
	L"先輩女子",  	L"Girl", 
	L"大男",  	L"Man", 
	L"担任",  	L"Teacher", 
	L"男",  	L"Man", 
	L"男子",  	L"Boy", 
	L"男子生徒",  	L"Schoolboy", 
	L"男子生徒Ａ",  	L"Schoolboy A", 
	L"男子生徒Ｂ",  	L"Schoolboy B", 
	L"男子生徒Ｃ",  	L"Schoolboy C", 
	L"猪",  	L"Boar", 
	L"津久野",  	L"Tsukuno", 
	L"電話",  	L"Phone", 
	L"電話の相手",  	L"Man on Phone", 
	L"二人",  	L"二人", 
	L"二年女子",  	L"Second-Year Girl", 
	L"二年男子",  	L"Second-Year Boy", 
	L"日本語文",  	L"Japanese Text", 
	L"入れ墨をした男",  	L"Tattoed Man", 
	L"猫",  	L"Cat", 
	L"不審人物",  	L"Suspicious Person", 
	L"福顔の男",  	L"Happy-Looking Man", 
	L"編集",  	L"Editor", 
	L"編集者",  	L"Editor", 
	L"鳳",  	L"Ootori", 
	L"魔女",  	L"Witch", 
	L"理香子",  	L"Rikako", 
	L"鈴木",  	L"Suzuki", 
	L"老婆",  	L"Old Woman", 
	L"洲崎",  	L"Suzaki"
};

typedef struct _NewlineAssociation
{
	int line;
	int offset;
	int type;
} NewlineAssociation;

vector<NewlineAssociation*> newlines;

int main(int argc, char **argv)
{
	FILE *infile = NULL, *outfile = NULL;
	Entry *strings = NULL;
	unsigned char *bytecode = NULL;
	int i = 0;
	
	if(argc!=3)
	{
		printf("usage: %s inscript outtext\n",argv[0]);
		return 0;
	}
	
	infile = fopen(argv[1],"rb");
	if(!infile)
	{
		printf("Could not open %s\n",argv[1]);
		return -1;
	}
	
	outfile = fopen(argv[2],"wb");
	fwrite("\xff\xfe",1,2,outfile);
	
	setlocale(LC_ALL, "Japanese");
	
	fread(&scrhead,sizeof(scrhead),1,infile);
	
	bytecode = (unsigned char*)calloc(scrhead.bytecode.size,sizeof(unsigned char));
	fseek(infile,scrhead.bytecode.offset,SEEK_SET);
	fread(bytecode,1,scrhead.bytecode.size,infile);
	
	fwprintf(outfile,L"// Newline command format: {0000} (newline type)\r\n// 0d = append\r\n// 0e = new page\r\n// anything else is unknown\r\n\r\n");
	
	for(i = 0; i < scrhead.bytecode.size; i++)
	{
		static unsigned char pushString[] = { 0x02, 0x14, 0x00, 0x00, 0x00 };
		static unsigned char pushNewline[] = { 0x02, 0x0a, 0x00, 0x00, 0x00 };
		static unsigned char pushNewline2[] = { 0x30, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 03, 00, 00, 00, 00 };
		int x = 0;
		
		if(memcmp(bytecode+i, pushString, 5) == 0)
		{
			unsigned int stringNum = *(unsigned int*)(bytecode+i+5);
			unsigned int pushType = 0;
			int foundPush = 0;
				
			for(x = i + 5; x < scrhead.bytecode.size - 5; x++)
			{
				if(x - i > 0x100)
					break;
					
				if(memcmp(bytecode+x, pushString, 5) == 0)
					break;
					
				if(memcmp(bytecode+x, pushNewline, 5) == 0 && memcmp(bytecode+x+9, pushNewline2, 0x16) == 0)
				{
					foundPush = 1;
					pushType = *(unsigned int*)(bytecode+x+5);
					break;
				}
			}	
				
			if(foundPush && pushType != 0x0e)
			{
				//printf("Found match at %08x (#%d) (%02x) // line %d\n",scrhead.bytecode.offset+i,stringNum, pushType, stringNum);
				//fwprintf(outfile,L"{%04x} %02x // line <%04d>\r\n", x+5, pushType, stringNum);
				
				NewlineAssociation *newline = (NewlineAssociation*)calloc(1, sizeof(NewlineAssociation));
				newline->line = stringNum;
				newline->offset = x+5;
				newline->type = pushType;
				newlines.push_back(newline);
			}
			else
			{
				//printf("Could not find push\n");
			}
			//exit(1);
		}	
	}
	
	strings = (Entry*)calloc(scrhead.strindex.size,sizeof(Entry));
	fseek(infile,scrhead.strindex.offset,SEEK_SET);
	fread(strings,sizeof(Entry),scrhead.strindex.size,infile);
	
	for(i=0; i<scrhead.strindex.size; i++)
	{
		wchar_t *line = (wchar_t*)calloc(strings[i].size+1,sizeof(wchar_t));
		wchar_t *lineparsed = (wchar_t*)calloc(strings[i].size*2,sizeof(wchar_t));
		int x = 0, j = 0, l = 0;
		int key = 0;
		
		fseek(infile,scrhead.strtable.offset+strings[i].offset*sizeof(wchar_t),SEEK_SET);
		fread(line,sizeof(wchar_t),strings[i].size,infile);
		
		// for v2.00 of rewrite trial (and probably newer siglusengine stuff)
		key = i * 0x7087;
		for(l=0; l<strings[i].size; l++)
		{
			line[l] ^= key; 
		}
		
		fwprintf(outfile,L"//");
		for(l=0; l<2; l++)
		{
			fwprintf(outfile,L"<%04d> ",i);
			
			if(l > 0)
			{
				int nameFound = 0;
				
				for(x = 0; x < nameArrayLen; x+=2)
				{
					if(wcscmp(line,nameArray[x])==0)
					{
						fwprintf(outfile,L"%s",nameArray[x+1]);
						nameFound = 1;
						break;
					}
				}
				
				if(nameFound)
				{
					fwprintf(outfile,L"\r\n");
					break;
				}
			}
			
			for(x=0,j=0; x<strings[i].size; j++, x++)
			{
				if(line[x]=='\n')
					fwprintf(outfile,L"\\n");
				else
					fwprintf(outfile,L"%c",line[x]);
			}
		
			fwprintf(outfile,L"\r\n");
		}
		
		for(x = 0; x < newlines.size(); x++)
		{
			if(newlines[x]->line == i)
			{
				//printf("Found match: {%04x} %02x // line <%04d>\n", newlines[x]->offset, newlines[x]->type, newlines[x]->line);
				fwprintf(outfile,L"{%04x} %02x // %04d\r\n", newlines[x]->offset, newlines[x]->type, newlines[x]->line);
				break;
			}
		}
		fwprintf(outfile,L"\r\n");
		
		free(line);
	}
	
	return 0;
}
