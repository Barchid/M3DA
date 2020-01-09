#include <iostream>
#include "wavdata.h"
#include "fft.h"
#include <math.h>

#define DELAY 5000
#define AMPLITUDE 0.5

int main(int argc, char **argv)
{
    // Exemple de base
    WavData w;
    w.load("COW.WAV");

    char *data = w.data();
    // data2 est un buffer deux fois plus long que le cow.wav original
    char *data2 = new char[w.datasize()*2];

    int i;
    for(i=0;i<w.datasize();i++)
        // recopier cow.wav dans la première moitié de data2
        data2[i]=data[i];
    for(;i<w.datasize()*2;i++)
        // Mettre des 128 de gitans pour la deuxième moitié de data2
        data2[i]=128;


    for(int i=DELAY;i<2*w.datasize();i++)
    {
        float value = (float)(unsigned char)data2[i-DELAY]-128.0;
        value = value * AMPLITUDE;
        int val = (unsigned int)value + (unsigned char)data2[i];
        if(val>255)val=255;
        if(val<0)val=0;

        data2[i]= (unsigned char)(unsigned int)val;
    }

    w.clearData();
    w.setDatasize(w.datasize()*2);
    w.setData(data2);

    w.save("COW_d.WAV");


    // Créer un song pur (le "la" de 440hz)
    //    WavData w;
    //    w.load("COW.WAV");

    //    char *data = w.data();
    //    // "son" est un buffer deux fois plus long que le cow.wav original
    //    char *son = new char[w.datasize()];

    //    float frequence = 440.; // 440 hz pour le "la" de référence --> on doit faire 440 tour par seconde

    //    // on veut faire 440 oscillations par secondes selon un échantillonnage défini par le WaveData
    //    float oscillation = ((2*M_PI) / w.frequency()) * frequence;

    //    //float oscillation = (2 * M_PI) / (frequence * w.frequency());

    //    // Placer les fréquences pour faire un "la"
    //    int i;
    //    for(i = 0 ; i < w.datasize(); i++) {
    //        float f = sin(i * oscillation); // fréquence pour l'itération courante
    //        int val_son = 128 * f + 128; // on place f dans intervalle [0, 255] (but de l'amplitude)
    //        son[i] = (unsigned char) (unsigned int) (val_son);

    //        std::cout << val_son << std::endl;
    //    }
    //    //std::cout << w.frequency() << std::endl;

    //    w.clearData();
    //    w.setDatasize(w.datasize());
    //    w.setData(son);

    //    w.save("COW_d.WAV");
}
