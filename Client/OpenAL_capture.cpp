/////////////////////////////////////////////////////////////////////
///  CODE ORIGINAL ISSUS DU TUTORIEL DE LAURENT GOMILA            ///
///  sur developpez.net, Effectuer des captures audio avec OpenAL ///
///								  ///
///  Modifi� pour �tre utilis� dans notre projet et               ///
///  permettre l'envoie de messages vocaux.		          ///
/////////////////////////////////////////////////////////////////////


#ifdef __APPLE__
  #include <OpenAL/al.h>
  #include <OpenAL/alc.h>
#else
  #include <al/al.h>
  #include <al/alc.h>
#endif
#include <sndfile.h>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <cstring>
#include <vector>
#include <cstdlib>
#include <pthread.h>

// Les diff�rents devices audio utilis�s
ALCdevice* Device        = NULL;
ALCdevice* CaptureDevice = NULL;

extern "C" int main_capture(); // pour appeler la fonction qui est du c++ dans nos fichiers .c


////////////////////////////////////////////////////////////
/// R�cup�re la liste des noms des devices de capture disponibles
///
/// \param Devices : Tableau de cha�nes � remplir avec les noms des devices
///
////////////////////////////////////////////////////////////
void GetCaptureDevices(std::vector<std::string>& Devices)
{
    // Vidage de la liste
    Devices.clear();

    // R�cup�ration des devices disponibles
    const ALCchar* DeviceList = alcGetString(NULL, ALC_CAPTURE_DEVICE_SPECIFIER);

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
        std::cerr << "Impossible de r�cup�rer la liste des devices de capture" << std::endl;
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
bool InitOpenAL_capture(const char* DeviceName = NULL)
{
    // Ouverture du device
    Device = alcOpenDevice(DeviceName);
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
/// Initialise la capture audio
///
/// \param DeviceName : Nom du device de capture � ouvrir (NULL pour le device par d�faut)
///
/// \return True si tout s'est bien pass�, false en cas d'erreur
///
////////////////////////////////////////////////////////////
bool InitCapture(const char* DeviceName = NULL)
{
    // On commence par v�rifier que la capture audio est support�e
    if (alcIsExtensionPresent(Device, "ALC_EXT_CAPTURE") == AL_FALSE)
    {
        std::cerr << "La capture audio n'est pas support�e par votre systeme" << std::endl;
        return false;
    }

    // Ouverture du device
    CaptureDevice = alcCaptureOpenDevice(DeviceName, 44100, AL_FORMAT_MONO16, 44100);
    if (!CaptureDevice)
    {
        std::cerr << "Impossible d'ouvrir le device de capture" << std::endl;
        return false;
    }

    return true;
}


////////////////////////////////////////////////////////////
/// Sauvegarde un tableau d'�chantillons dans un fichier audio
///
/// \param Filename : Nom du fichier audio � charger
/// \param Samples :  Tableau d'�chantillons
///
////////////////////////////////////////////////////////////
void SaveSound(const std::string& Filename, const std::vector<ALshort>& Samples)
{
    // On renseigne les param�tres du fichier � cr�er
    SF_INFO FileInfos;
    FileInfos.channels   = 1;
    FileInfos.samplerate = 44100;
    FileInfos.format     = SF_FORMAT_PCM_16 | SF_FORMAT_WAV;

    // On ouvre le fichier en �criture
    SNDFILE* File = sf_open(Filename.c_str(), SFM_WRITE, &FileInfos);
    if (!File)
    {
        std::cerr << "Impossible de cr�er le fichier audio" << std::endl;
        return;
    }

    // Ecriture des �chantillons audio
    sf_write_short(File, &Samples[0], Samples.size());

    // Fermeture du fichier
    sf_close(File);
}


////////////////////////////////////////////////////////////
/// Ferme proprement le device de capture
///
////////////////////////////////////////////////////////////
void ShutdownCapture()
{
    // Fermeture du device de capture
    alcCaptureCloseDevice(CaptureDevice);
}


////////////////////////////////////////////////////////////
/// Ferme proprement OpenAL
///
////////////////////////////////////////////////////////////
void ShutdownOpenAL_capture()
{
    // R�cup�ration du contexte
    ALCcontext* Context = alcGetCurrentContext();

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
int main_capture()
{
    int time_left = 0;

    // Initialisation d'OpenAL avec le device par d�faut
    if (!InitOpenAL_capture(NULL))
        return EXIT_FAILURE;

    // Initialisation de la capture
    if (!InitCapture(NULL))
        return EXIT_FAILURE;

    // Lancement de la capture
    alcCaptureStart(CaptureDevice);

    // On va stocker les �chantillons captur�s dans un tableau d'entiers sign�s 16 bits
    std::vector<ALshort> Samples;

    // ...Et c'est parti pour 5 secondes de capture
    time_t Start = time(NULL);

    while ((time_left = time(NULL) - Start) <= 5) // si x passe � 1, on stop la capture
    {
	std::cout << "\rSpeech capture in progress... " << std::fixed << std::setprecision(2) << 5-time_left << " sec left";
        // On r�cup�re le nombre d'�chantillons disponibles
        ALCint SamplesAvailable;
        alcGetIntegerv(CaptureDevice, ALC_CAPTURE_SAMPLES, 1, &SamplesAvailable);

        // On lit les �chantillons et on les ajoute au tableau
        if (SamplesAvailable > 0)
        {
            std::size_t Start = Samples.size();
            Samples.resize(Start + SamplesAvailable);
            alcCaptureSamples(CaptureDevice, &Samples[Start], SamplesAvailable);
        }
    }

    // On stoppe la capture
    alcCaptureStop(CaptureDevice);

    // On n'oublie pas les �ventuels �chantillons qu'il reste � r�cup�rer
    ALCint SamplesAvailable;
    alcGetIntegerv(CaptureDevice, ALC_CAPTURE_SAMPLES, 1, &SamplesAvailable);
    if (SamplesAvailable > 0)
    {
        std::size_t Start = Samples.size();
        Samples.resize(Start + SamplesAvailable);
        alcCaptureSamples(CaptureDevice, &Samples[Start], SamplesAvailable);
    }

	std::cout << "\rSpeech capture complete                   " << std::endl;

    // On sauvegarde les �chantillons captur�s dans un fichier
    SaveSound("vocal.wav", Samples);

    // Fermeture de la capture
    ShutdownCapture();

    // Fermeture d'OpenAL
    ShutdownOpenAL_capture();

    return EXIT_SUCCESS;
}
