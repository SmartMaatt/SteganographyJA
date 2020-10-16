﻿#include "FileManager.h"

//Konstruktor klasy FileManager
FileManager::FileManager()
{
}

//Funkcja odczytująca pliki bmp lub txt w zależności od przekazanych przełączników
String^ FileManager::readFile(System::Object^ sender, System::EventArgs^ e, bool bmp)
{
	Stream^ myStream;
	OpenFileDialog^ dialog = gcnew OpenFileDialog();	//Utworzenie okna dialogowego

	if(bmp) dialog->Filter = "bmp files(*.bmp)|*.bmp";	//Filtr plików .bmp lub .txt w zależności od parametru
	else dialog->Filter = "txt files(*.txt)|*.txt";

	if (dialog->ShowDialog() == System::Windows::Forms::DialogResult::OK)	//Sprawdzenie załadowania okna dialogowego
	{
		if ((myStream = dialog->OpenFile()) != nullptr)	//Sprawdzenie czy okno dialogowe jest otwarte
		{
			String^ file_adres = dialog->InitialDirectory + dialog->FileName;	//Odczyt adres pliku
			myStream->Close();	//Zamknięcie strumienia

			if (bmp) { if (!readBmp(file_adres)) return "Error 3"; }	//Bezpośredni odczyt
			else { if (!readTxt(file_adres)) return "Error 3"; }
		}
		else return "Error1";
	}
	else return "Error2";

	return dialog->FileName;	//Zwrócenie błędów lub ścieżki do pliku
}


//Odczyt plików bmp w wykorzystaniu opcji języka C (bo najbardziej wolę)
bool FileManager::readBmp(String^ adres)
{
	string loaded_adres_std = msclr::interop::marshal_as<string>(adres);	//Konwersja System::String na std::string
	bool success = false;	//Zmienna definiująca wynik operacji
	const char *cfilename = loaded_adres_std.c_str();	//Zapis ścieżki do pliku do char*

	FILE* f;	//Zmienna plikowa
	fopen_s(&f, cfilename, "rb");	//Otwacie w trybie "rb"
	this->headerInfo = new char[54]; // alokowanie 54 bajtów na header pliku
	fread(this->headerInfo, sizeof(char), 54, f); // zczytanie headera

	// informacje o rozmiarze obrazu z headera
	width = *(int*)&headerInfo[18];
	height = *(int*)&headerInfo[22];

	size = 3 * width * height;
	if (size <= 1920 * 1080 * 3 && size > 9)	//Obraz nie może być większy od FHD
	{
		bmpData = new char[size]; // 3 bajty na piksel
		fread(bmpData, sizeof(char), size, f); // zczytanie reszty pliku
		success = true;
	}
	else delete[] headerInfo; //Zwolnienie pamięci headerInfo

	fclose(f); // zamkniêcie pliku
	return success;
}


//Odczyt plików txt z wykorzystaniem możliwości System::IO
bool FileManager::readTxt(String^ adres)
{
	bool succes = false;	//Status operacji
	String^ loaded_file = File::ReadAllText(adres);	//Oczyt całości pliku do zmiennej
	string loaded_file_std = msclr::interop::marshal_as<string>(loaded_file);	//Konwersja System::String na std::string

	if(loaded_file_std.length() < (this->size)/8)	//Sprawdzenie czy tekst zmieści się w zdjęciu
	{
		this->txtData = new char[loaded_file_std.length() + 1];	//Alokacja miejsca na tekst
		this->txtData[loaded_file_std.length() + 1] = '\0';	//Dodanie znaku końca łańcucha
		this->txtLenght = loaded_file_std.length();	//Zapis długości tekstu
		succes = true;
	}

	return succes;	//Zwracanie statusu sytuacji
}