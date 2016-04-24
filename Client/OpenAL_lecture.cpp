//////////////////////////////////////////////////////////////////
///  CODE ORIGINAL ISSUS DU TUTORIEL DE LAURENT GOMILA         ///
///  sur developpez.net, Premiers pas avec l'API audio OpenAL  ///
///                                                            ///
///  Modifié pour être utilisé dans notre projet et            ///
///  permettre la lecture de messages vocaux.		       ///
//////////////////////////////////////////////////////////////////

#include <AL/al.h>
#include <AL/alc.h>
#include <sndfile.h>
#include <iomanip>
#include <iostream>
#include <cstring>
#include <vector>
#include <cstdlib>


extern "C" int main_lecture(char *fichier); // pour appeler la fonction qui est du c++ dans nos fichiers .c

////////////////////////////////////////////////////////////
/// Récupère la liste des noms des devices disponibles
///
/// \param Devices : Tableau de chaînes à remplir avec les noms des devices
///
////////////////////////////////////////////////////////////
void GetDevices(std::vector<std::string>& Devices)
{
    // Vidage de la liste
    Devices.clear();

    // Récupération des devices disponibles
    const ALCchar* DeviceList = alcGetString(NULL, ALC_DEVICE_SPECIFIER);

    if (DeviceList)
    {
        // Extraction des devices contenus dans la chaîne renvoyée
        while (strlen(DeviceList) > 0)
        {
            Devices.push_back(DeviceList);
            DeviceList += strlen(DeviceList) + 1;
        }
    }
    else
    {
        std::cerr << "Impossible de récupérer la liste des devices" << std::endl;
    }
}


////////////////////////////////////////////////////////////
/// Initialise OpenAL (ouvre un device et crée un contexte audio)
///
/// \param DeviceName : Nom du device à ouvrir (NULL pour le device par défaut)
///
/// \return True si tout s'est bien passé, false en cas d'erreur
///
////////////////////////////////////////////////////////////
bool InitOpenAL_lecture(const char* DeviceName = NULL)
{
    // Ouverture du device
    ALCdevice* Device = alcOpenDevice(DeviceName);
    if (!Device)
    {
        std::cerr << "Impossible d'ouvrir le device par défaut" << std::endl;
        return false;
    }

    // Création du contexte
    ALCcontext* Context = alcCreateContext(Device, NULL);
    if (!Context)
    {
        std::cerr << "Impossible de créer un contexte audio" << std::endl;
        return false;
    }

    // Activation du contexte
    if (!alcMakeContextCurrent(Context))
    {
        std::cerr << "Impossible d'activer le contexte audio" << std::endl;
        return false;
    }

    return true;
}


////////////////////////////////////////////////////////////
/// Crée un tampon OpenAL à partir d'un fichier audio
///
/// \param Filename : Nom du fichier audio à charger
///
/// \return Identificateur du tampon OpenAL (0 si échec)
///
////////////////////////////////////////////////////////////
ALuint LoadSound(const std::string& Filename)
{
    // Ouverture du fichier audio avec libsndfile
    SF_INFO FileInfos;
    SNDFILE* File = sf_open(Filename.c_str(), SFM_READ, &FileInfos);
    if (!File)
    {
        std::cerr << "Impossible d'ouvrir le fichier audio" << std::endl;
        return 0;
    }

    // Lecture du nombre d'échantillons et du taux d'échantillonnage (nombre d'échantillons à lire par seconde)
    ALsizei NbSamples  = static_cast<ALsizei>(FileInfos.channels * FileInfos.frames);
    ALsizei SampleRate = static_cast<ALsizei>(FileInfos.samplerate);

    // Lecture des échantillons audio au format entier 16 bits signé (le plus commun)
    std::vector<ALshort> Samples(NbSamples);
    if (sf_read_short(File, &Samples[0], NbSamples) < NbSamples)
    {
        std::cerr << "Impossible de lire les échantillons stockés dans le fichier audio" << std::endl;
        return 0;
    }

    // Fermeture du fichier
    sf_close(File);

    // Détermination du format en fonction du nombre de canaux
    ALenum Format;
    switch (FileInfos.channels)
    {
        case 1 : Format = AL_FORMAT_MONO16;   break;
        case 2 : Format = AL_FORMAT_STEREO16; break;
        default :
            std::cerr << "Format audio non supporté (plus de 2 canaux)" << std::endl;
            return 0;
    }

    // Création du tampon OpenAL
    ALuint Buffer;
    alGenBuffers(1, &Buffer);

    // Remplissage avec les échantillons lus
    alBufferData(Buffer, Format, &Samples[0], NbSamples * sizeof(ALushort), SampleRate);

    // Vérification des erreurs
    if (alGetError() != AL_NO_ERROR)
    {
        std::cerr << "Impossible de remplir le tampon OpenAL avec les échantillons du fichier audio" << std::endl;
        return 0;
    }

    return Buffer;
}


////////////////////////////////////////////////////////////
/// Ferme proprement OpenAL
///
////////////////////////////////////////////////////////////
void ShutdownOpenAL_lecture()
{
    // Récupération du contexte et du device
    ALCcontext* Context = alcGetCurrentContext();
    ALCdevice*  Device  = alcGetContextsDevice(Context);

    // Désactivation du contexte
    alcMakeContextCurrent(NULL);

    // Destruction du contexte
    alcDestroyContext(Context);

    // Fermeture du device
    alcCloseDevice(Device);
}


////////////////////////////////////////////////////////////
/// Point d'entrée du programme
///
/// \return Code d'erreur de l'application
///
////////////////////////////////////////////////////////////
int main_lecture(char *fichier)
{
    // Initialisation d'OpenAL
    InitOpenAL_lecture(NULL); //NULL = device par défault

    // Chargement du fichier audio
    ALuint Buffer = LoadSound(fichier);
    if (Buffer == 0)
        return EXIT_FAILURE;

    // Création d'une source
    ALuint Source;
    alGenSources(1, &Source);
    alSourcei(Source, AL_BUFFER, Buffer);

    // On joue le son
    alSourcePlay(Source);

    // On attend qu'il soit terminé
    ALint Status;
    do
    {
        // Récupération de l'état du son
        alGetSourcei(Source, AL_SOURCE_STATE, &Status);
    }
    while (Status == AL_PLAYING);

    // Destruction du tampon
    alDeleteBuffers(1, &Buffer);

    // Destruction de la source
    alSourcei(Source, AL_BUFFER, 0);
    alDeleteSources(1, &Source);

    // Fermeture d'OpenAL
    ShutdownOpenAL_lecture();

    return EXIT_SUCCESS;
}
