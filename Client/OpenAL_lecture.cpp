//////////////////////////////////////////////////////////////////
///  CODE ORIGINAL ISSUS DU TUTORIEL DE LAURENT GOMILA         ///
///  sur developpez.net, Premiers pas avec l'API audio OpenAL  ///
///                                                            ///
///  Modifi� pour �tre utilis� dans notre projet et            ///
///  permettre la lecture de messages vocaux.		       ///
//////////////////////////////////////////////////////////////////

#ifdef __APPLE__
  #include <OpenAL/al.h>
  #include <OpenAL/alc.h>
#else
  #include <al/al.h>
  #include <al/alc.h>
#endif
#include <sndfile.h>
#include <iomanip>
#include <iostream>
#include <cstring>
#include <vector>
#include <cstdlib>


extern "C" int main_lecture(char *fichier); // pour appeler la fonction qui est du c++ dans nos fichiers .c

////////////////////////////////////////////////////////////
/// R�cup�re la liste des noms des devices disponibles
///
/// \param Devices : Tableau de cha�nes � remplir avec les noms des devices
///
////////////////////////////////////////////////////////////
void GetDevices(std::vector<std::string>& Devices)
{
    // Vidage de la liste
    Devices.clear();

    // R�cup�ration des devices disponibles
    const ALCchar* DeviceList = alcGetString(NULL, ALC_DEVICE_SPECIFIER);

    if (DeviceList)
    {
        // Extraction des devices contenus dans la cha�ne renvoy�e
        while (strlen(DeviceList) > 0)
        {
            Devices.push_back(DeviceList);
            DeviceList += strlen(DeviceList) + 1;
        }
    }
    else
    {
        std::cerr << "Impossible de r�cup�rer la liste des devices" << std::endl;
    }
}


////////////////////////////////////////////////////////////
/// Initialise OpenAL (ouvre un device et cr�e un contexte audio)
///
/// \param DeviceName : Nom du device � ouvrir (NULL pour le device par d�faut)
///
/// \return True si tout s'est bien pass�, false en cas d'erreur
///
////////////////////////////////////////////////////////////
bool InitOpenAL_lecture(const char* DeviceName = NULL)
{
    // Ouverture du device
    ALCdevice* Device = alcOpenDevice(DeviceName);
    if (!Device)
    {
        std::cerr << "Impossible d'ouvrir le device par d�faut" << std::endl;
        return false;
    }

    // Cr�ation du contexte
    ALCcontext* Context = alcCreateContext(Device, NULL);
    if (!Context)
    {
        std::cerr << "Impossible de cr�er un contexte audio" << std::endl;
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
/// Cr�e un tampon OpenAL � partir d'un fichier audio
///
/// \param Filename : Nom du fichier audio � charger
///
/// \return Identificateur du tampon OpenAL (0 si �chec)
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

    // Lecture du nombre d'�chantillons et du taux d'�chantillonnage (nombre d'�chantillons � lire par seconde)
    ALsizei NbSamples  = static_cast<ALsizei>(FileInfos.channels * FileInfos.frames);
    ALsizei SampleRate = static_cast<ALsizei>(FileInfos.samplerate);

    // Lecture des �chantillons audio au format entier 16 bits sign� (le plus commun)
    std::vector<ALshort> Samples(NbSamples);
    if (sf_read_short(File, &Samples[0], NbSamples) < NbSamples)
    {
        std::cerr << "Impossible de lire les �chantillons stock�s dans le fichier audio" << std::endl;
        return 0;
    }

    // Fermeture du fichier
    sf_close(File);

    // D�termination du format en fonction du nombre de canaux
    ALenum Format;
    switch (FileInfos.channels)
    {
        case 1 : Format = AL_FORMAT_MONO16;   break;
        case 2 : Format = AL_FORMAT_STEREO16; break;
        default :
            std::cerr << "Format audio non support� (plus de 2 canaux)" << std::endl;
            return 0;
    }

    // Cr�ation du tampon OpenAL
    ALuint Buffer;
    alGenBuffers(1, &Buffer);

    // Remplissage avec les �chantillons lus
    alBufferData(Buffer, Format, &Samples[0], NbSamples * sizeof(ALushort), SampleRate);

    // V�rification des erreurs
    if (alGetError() != AL_NO_ERROR)
    {
        std::cerr << "Impossible de remplir le tampon OpenAL avec les �chantillons du fichier audio" << std::endl;
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
    // R�cup�ration du contexte et du device
    ALCcontext* Context = alcGetCurrentContext();
    ALCdevice*  Device  = alcGetContextsDevice(Context);

    // D�sactivation du contexte
    alcMakeContextCurrent(NULL);

    // Destruction du contexte
    alcDestroyContext(Context);

    // Fermeture du device
    alcCloseDevice(Device);
}


////////////////////////////////////////////////////////////
/// Point d'entr�e du programme
///
/// \return Code d'erreur de l'application
///
////////////////////////////////////////////////////////////
int main_lecture(char *fichier)
{
    // Initialisation d'OpenAL
    InitOpenAL_lecture(NULL); //NULL = device par d�fault

    // Chargement du fichier audio
    ALuint Buffer = LoadSound(fichier);
    if (Buffer == 0)
        return EXIT_FAILURE;

    // Cr�ation d'une source
    ALuint Source;
    alGenSources(1, &Source);
    alSourcei(Source, AL_BUFFER, Buffer);

    // On joue le son
    alSourcePlay(Source);

    // On attend qu'il soit termin�
    ALint Status;
    do
    {
        // R�cup�ration de l'�tat du son
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
