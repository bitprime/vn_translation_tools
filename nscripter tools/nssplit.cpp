#include <iostream>
#include <fstream>
#include <string>

using namespace std;

void main(void)
{
    ifstream script;
    script.open("0.txt");
    
    if(script.is_open())
    {
        int i=0;
        string line;
        ofstream outscript, outlist;
        
        outscript.open("_head.txt");
        outlist.open("_list.txt");
        
        outlist << "_head.txt" << endl;
        
        while(script.good())
        {
            getline(script, line);
        
            if(line[0]=='*')
            {
                char *labelname = new char[line.length() + 4];
                
                for(i=1; i<line.length() && line[i] != ';'; i++)
                    labelname[i-1] = line[i];
                labelname[i-1] = '\0';
				
				while(strlen(labelname) > 0 &&
					labelname[strlen(labelname)-1]==' ' ||
					labelname[strlen(labelname)-1]=='\t' ||
					labelname[strlen(labelname)-1]=='\n' ||
					labelname[strlen(labelname)-1]=='\r')
						labelname[strlen(labelname)-1] = '\0';
				
				strcat(labelname,".txt\0");
				line += ".txt\0";
				
                outlist << labelname << endl;
				cout << labelname << endl;
                
                outscript.close();
                outscript.open(labelname);
                
                delete [] labelname;
            }
            else
            {
                outscript << line << endl;
            }
        }
    }
}