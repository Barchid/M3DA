#ifndef WAVDATA_H
#define WAVDATA_H

#include <iostream>
#include <fstream>

class WavData
{
	unsigned int _audioFormat;
	unsigned int _nbrChanel;
	unsigned int _frequency;
	unsigned int _bytePerBloc;
	unsigned int _bytePerSec;
	unsigned int _bitsPerSample;

	char * _data;
	unsigned int _datasize;
public:
	WavData();

	int load(char* s);
	int save(char* s);

	unsigned int fromLittleEndian(char * content, int size);
	char * toLittleEndian(unsigned int content, int size);
	void toLittleEndian(char* result,unsigned int content, int size);

	inline unsigned int audioFormat(){return _audioFormat;}
	inline unsigned int nbrChanel(){return _nbrChanel;}
	inline unsigned int frequency(){return _frequency;}
	inline unsigned int bytePerBloc(){return _bytePerBloc;}
	inline unsigned int bytePerSec(){return _bytePerSec;}
	inline unsigned int bitsPerSample(){return _bitsPerSample;}
	inline unsigned int datasize(){return _datasize;}
	inline char * data(){return _data;}

	inline void setAudioFormat(unsigned int a){_audioFormat=a;}
	inline void setNbrChanel(unsigned int a){_nbrChanel=a;}
	inline void setFrequency(unsigned int a){_frequency=a;}
	inline void setBytePerBloc(unsigned int a){_bytePerBloc=a;}
	inline void setBytePerSec(unsigned int a){_bytePerSec=a;}
	inline void setBitsPerSample(unsigned int a){_bitsPerSample=a;}
	inline void setData(char * a){_data=a;}
	inline void clearData(){delete[] _data;_data=NULL;}
	inline void setDatasize(unsigned int i){_datasize=i;}
	
private:
	int error(int,int);
	int openFormatBloc(std::ifstream* file);
	int openDescriptionBloc(std::ifstream *file);
	int openDataBloc(std::ifstream *file);

	int saveFormatBloc(std::ofstream *file);
	int saveDescriptionBloc(std::ofstream *file);
	int saveDataBloc(std::ofstream *file);
	
	void write(std::ofstream *file,char* mess,unsigned int size);
	char * read(std::ifstream* file,unsigned int size);
	
	bool comp(char* c1, char *c2, int size);
	int testString(std::ifstream* file,char*compstr,int size,char *mess);
	int testInt(std::ifstream* file,int size,char *mess,unsigned int *val=NULL);

};

#endif // WAVDATA_H
