#include "wavdata.h"
#include <iostream>


using namespace std;

WavData::WavData()
{
	_data=NULL;
}

int WavData::error(int i,int line)
{
	std::cerr << "Erreur "<<__FILE__<<" L"<<line<< ": ";
	switch(i)
	{
	case -1: std::cerr << "ouverture du fichier";
		break;
	case -2: std::cerr << "le fichier n'est pas au format WAVE";
		break;
	default: std::cerr << "inconnue";
		break;
	}

	std::cerr << "\n";
	return i;
}

unsigned int WavData::fromLittleEndian(char * content, int size)
{
	unsigned int result = 0;
	for(int i=size;i>=0;i--)
	{
		result *= 256;
		result += (unsigned char)content[i];
	}
	return result;
}

char * WavData::toLittleEndian(unsigned int content, int size)
{
	char * result= new char [size*sizeof(char)];

	for(int i=0;i<size;i++)
	{
		result += (char)(content%256);
		content >>= 8;
	}

	return result;
}

char * WavData::read(ifstream* file,unsigned int size)
{
	char * c = new char[size+1];
	for(int i=0;i<size;i++)
	{
		c[i] = file->get();
	}
	c[size]='\0';
	return c;
}

bool WavData::comp(char* c1, char *c2, int size)
{
	for(int i=0;i<size;i++)
	{
		if(c1[i]!=c2[i])return false;
	}
	return true;
}

int WavData::testString(ifstream* file,char*compstr,int size,char *mess)
{
	char * content = read(file,size);
	std::cout << "| "<< mess << std::endl<<"|    ->"<< content << std::endl << "|"<<std::endl;
	if(!comp(content,compstr,size))return error(-2,__LINE__);
	
	delete[] content;
	return 1;
}

int WavData::testInt(ifstream* file,int size,char *mess,unsigned int *val)
{
	int test;
	char * content = read(file,size);
	
	int value=fromLittleEndian(content, size);
	std::cout << "| "<< mess << std::endl<<"|    ->"<<value << std::endl<<"|" <<std::endl;
	if(val!=NULL)*val=value;
	delete[] content;
	return 1;
}

int WavData::openFormatBloc(ifstream* file)
{
	std::cout << "[Bloc de declaration d'un fichier au format WAVE]"<<std::endl<<"|"<<std::endl;

	//  FileTypeBlocId
	int r;
	r = testString(file,"RIFF",4,	"FileTypeBlocID  (4 octets) : Constante 'RIFF'");
	if(r<=0)return r;
	
	//  FileSize
	r = testInt(file,4,				"FileSize        (4 octets) : Taille du fichier moins 8 octets");
	if(r<=0)return r;
	
	//  FileFormatID
	r = testString(file,"WAVE",4,	"FileFormatID    (4 octets) : Format = 'WAVE'");
	if(r<=0)return r;
	
	std::cout << std::endl;
	return 1;
}

int WavData::openDescriptionBloc(ifstream* file)
{
	std::cout << "[Bloc decrivant le format audio]"<<std::endl<<"|"<<std::endl;
	
	int r;

	//  FormatBlocID
	r = testString(file,"fmt ",4, "FormatBlocID    (4 octets) : Identifiant 'fmt'");
	if(r<=0)return r;

	//  BlocSize
	r = testInt(file,4,"BlocSize        (4 octets) : Nombre d'octets du bloc - 8 (attendue: 16)");
	if(r<=0)return r;
	
	//  AudioFormat
	r = testInt(file,2,"AudioFormat     (2 octets) : Format du stockage dans le fichier (1: PCM, ...)",&_audioFormat);
	if(r<=0)return r;
	
	//  NbrCanaux
	r = testInt(file,2,"NbrCanaux       (2 octets) : Nombre de canaux (de 1 a 6)",&_nbrChanel);
	if(r<=0)return r;
	
	//  Frequency
	r = testInt(file,4,"Frequence       (4 octets) : Frequence d'echantillonnage (en Hertz)",&_frequency);
	if(r<=0)return r;
	
	//  BytePerSec
	r = testInt(file,4,"BytePerSec      (4 octets) : Nombre d'octets a lire par seconde",&_bytePerSec);
	if(r<=0)return r;
	
	//   BytePerBloc
	r = testInt(file,2,"BytePerBloc     (2 octets) : Nombre d'octets par bloc d'echantillonnage",&_bytePerBloc);
	if(r<=0)return r;
	
	//   BitsPerSample
	r = testInt(file,2,"BitsPerSample   (2 octets) : Nombre de bits pour le codage d'un echantillon",&_bitsPerSample);
	if(r<=0)return r;

	return 1;
}

int WavData::openDataBloc(ifstream* file)
{
	std::cout << "[Bloc des donnees]\n";
	
	int r,test;
	//  DataBlocID
	r = testString(file,"data",4,"DataBlocID      (4 octets) : Constante 'data'");
	if(r<=0)return r;
	
	//  DataSize
	r = testInt(file,3,"DataSize        (4 octets) : Nombre d'octets des donnees",&_datasize);
	if(r<=0)return r;
	
	//  Data[];
	std::cout << "| Data[] : [Octets du Sample 1 du Canal 1] [Octets du Sample 1 du Canal 2] [Octets du Sample 2 du Canal 1] [Octets du Sample 2 du Canal 2]\n\n";
	if(_data!=NULL)delete[] _data;
	_data = read(file,_datasize);
	
	return 1;
}

int WavData::load(char * s)
{
	std::cout << "Load file: \"" << s << "\""<<std::endl<<std::endl;

	ifstream file(s, ios::in | ios::binary );
	if (!file)return error(-1,__LINE__);

	if(openFormatBloc(&file)		!= 1)	{file.close(); return 0;}
	if(openDescriptionBloc(&file)	!= 1)	{file.close(); return 0;}
	if(openDataBloc(&file)			!= 1)	{file.close(); return 0;}
	
	std::cout << "close file"<< std::endl;
	file.close();
	std::cout << "file closed"<<std::endl;

	return 1;
}

void WavData::toLittleEndian(char* result,unsigned int content, int size)
{
	for(int i=0;i<size;i++)
	{
		result[i] = (char)(content%256);
		content >>= 8;
	}

	return;
}

void WavData::write(ofstream *file,char* mess,unsigned int size)
{
	for(int i=0;i<size;i++)
	{
		file->put(mess[i]);
	}
}

int WavData::saveFormatBloc(ofstream* file)
{
	char b[13] = "RIFF    WAVE";
	
	toLittleEndian(b+4,_datasize+36,4);
	
	write(file,b,12);
	
	return 1;
}

int WavData::saveDescriptionBloc(ofstream* file)
{
	char b[25] = "fmt                     ";
	
	toLittleEndian(b+4,0x10,4);
	std::cout<<_audioFormat<<std::endl;
	toLittleEndian(b+8,_audioFormat,2);
	toLittleEndian(b+10,_nbrChanel,2);
	toLittleEndian(b+12,_frequency,4);
	toLittleEndian(b+16,_bytePerSec,4);
	toLittleEndian(b+20,_bytePerBloc,2);
	toLittleEndian(b+22,_bitsPerSample,2);
	
	write(file,b,24);
	
	return 1;
}
int WavData::saveDataBloc(ofstream* file)
{
	char b[9] = "data    ";
	toLittleEndian(b+4,_datasize,4);
	
	write(file,b,8);
	
	write(file,_data,_datasize);
	
	return 1;
}

int WavData::save(char * s)
{
	std::cout << "Save file: \"" << s << "\"\n\n";

	ofstream file(s, ios::out | ios::trunc | ios::binary);
	if (!file)return error(-1,__LINE__);

	saveFormatBloc(&file);
	saveDescriptionBloc(&file);
	saveDataBloc(&file);

	file.close();

	return 1;
}
