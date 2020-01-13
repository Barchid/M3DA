#include <iostream>
#include "wavdata.h"
#include "fft.h"
#include <math.h>

#define DELAY 5000
#define AMPLITUDE 0.5

/**
 * @brief creerSonPur crée un son pur avec un nombre d'oscillation défini en paramètre et d'une taille définie aussi.
 * Ressource qui a aidée : https://medium.com/@audiowaves/lets-write-a-simple-sine-wave-generator-with-c-and-juce-c8ab42d1f54f
 * @param frequence le nombre d'oscillations par seconde du son pur (en Hz)
 * @param size taille de la nouvelle
 * @param sampleRate l'échantillonnage utilisé (en Hz)
 * @return la chaine de char représente le son produit
 */
char* creerSonPur(float frequence, unsigned int size, float sampleRate) {
    char* sonPur = new char[size];

    // Déclaration des variables
    float amplitude = 128.; // amplitude est comprise ici entre -128 et 128 (c'est le volume en gros)
    float time = 0; // compteur pour compter le temps pendant la construction itérative
    float deltaTime = 1 / sampleRate; // on a le temps (en seconde) entre deux échantillonnages

    // Print pour créer le CSV
    std::cout << "\"Amplitude\",\"X\"" << std::endl;

    // Construction du son pur
    for(unsigned int i = 0; i < size; i++) {
        // Application de la formule pour l'échantillon au temps
        float echantillon = amplitude * sin(2 * M_PI * frequence * time);
        // placement de l'échantillon dans la chaine du son pur
        sonPur[i] = (unsigned char) (unsigned int) (echantillon);

        // Affichage en sortie standard
        std::cout << std::to_string(echantillon) + "," + std::to_string(i) << std::endl;

        // Mise à jour du compteur temps pour l'échantillon suivant
        time += deltaTime;
    }

    return sonPur;
}

/**
 * @brief augmenterDuree fonction qui double la durée du son en paramètre.
 * @param son le son à rallonger
 * @param size la taille actuelle du son
 * @return le nouveau son, allongé
 */
char* augmenterDuree(char* son, int size) {
    char* resultat = new char[size * 2];
    for(int i = 0; i < size; i++) {
        resultat[i*2] = son[i];
        resultat[i*2 + 1] = son[i];
    }
    return resultat;
}

int main(int argc, char **argv)
{
    // Chargement de la vache
    WavData w;
    w.load("COW.WAV");

    // Création du son pur
    unsigned int size = w.datasize() * 2;
    float frequence = 440.;
    float sampleRate = w.frequency();
    char* sonPur440 = creerSonPur(frequence, size, sampleRate);
    w.clearData();
    w.setDatasize(size);
    w.setData(sonPur440);
    w.save("Son_pur_440.WAV");

    // Création du son pur allongé (sans time stretching)
    char *dureeAugmentee = augmenterDuree(sonPur440, size);
    w.setDatasize(size * 2);
    w.clearData();
    w.setData(dureeAugmentee);
    w.save("Son_pur_440_augmente_sans_time_stretching.WAV");
//    // data2 est un buffer deux fois plus long que le cow.wav original
//    char *data2 = new char[w.datasize()*2];

//    int i;
//    for(i=0;i<w.datasize();i++)
//        // recopier cow.wav dans la première moitié de data2
//        data2[i]=data[i];
//    for(;i<w.datasize()*2;i++)
//        // Mettre des 128 de gitans pour la deuxième moitié de data2
//        data2[i]=128;


//    for(int i=DELAY;i<2*w.datasize();i++)
//    {
//        float value = (float)(unsigned char)data2[i-DELAY]-128.0;
//        value = value * AMPLITUDE;
//        int val = (unsigned int)value + (unsigned char)data2[i];
//        if(val>255)val=255;
//        if(val<0)val=0;

//        data2[i]= (unsigned char)(unsigned int)val;
//    }

//    w.clearData();
//    w.setDatasize(w.datasize()*2);
//    w.setData(data2);

//    w.save("COW_d.WAV");
}
