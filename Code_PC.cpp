//
//  main.cpp
//  Portfolio C++
//
//  Created by Julia Flatscher on 19.01.25.
// Dies ist ein Programm zur Berechnung einfacher mathematischer Ausdrücke.
// Vor dem Start muss die passende serille Schnittstelle sowie der gewünscht Dateipfad zur Speicherung der Berechnungen an kommentierter Stelle angepasst werden.
// Ein Arduino mit dem Programm Taschenrechner muss an der seriellen Schnittstelle angeschlossen sein.
// Die Baudrate im Programm muss gleich der des Arduino sein.

#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <cstring>
#include <vector>

using namespace std;

int main() {
//    Begrüßung der Programms
    cout << " Hallo, ich bin ein Programm zur Berechnung einfacher mathematischer Ausdrücke. Sie können beliebig viele Berechnungen durchführen und diese am Schluss speichern. Legen wir gleich los!\n";
    
    const char *serialPort = "/dev/cu.usbserial-B0000BTS"; // hier die serielle Schnittstelle anpassen
    int baudRate = 9600; // Baudrate festlegen
    vector<string> messageLog; // Berechnungen speichern

    while (true) {
        // Öffnen der seriellen Schnittstelle
        int serialFd = open(serialPort, O_RDWR | O_NOCTTY | O_NDELAY);
        // Fehlermeldung falls die serielle Schnittstelle nicht verfügbar ist
        if (serialFd == -1) {
            cerr << "Fehler: Die serielle Schnittstelle kann nicht geöffnet werden." << endl;
            return 1;
        }

        // seriellen Schnittstelle konfigurieren
       struct termios tty;


        cfsetispeed(&tty, baudRate);
        cfsetospeed(&tty, baudRate);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; // 8 Datenbits
        tty.c_cflag |= (CLOCAL | CREAD);           // Lokale Verbindung, Empfang aktiviert
        tty.c_cflag &= ~(PARENB | PARODD);         // Keine Parität
        tty.c_cflag &= ~CSTOPB;                    // 1 Stoppbit
        tty.c_cflag &= ~CRTSCTS;                   // Kein Hardware-Handshake

        tty.c_iflag = IGNPAR;                      // Ignoriere Paritätsfehler
        tty.c_oflag = 0;
        tty.c_lflag = 0;                           // Keine Verarbeitung



        // Rechenaufgabe abfragen
        double Zahl_1, Zahl_2;
        char op;
        int Wiederholungen = 5;

        // Erste Zahl einlesen
        while (Wiederholungen > 0) {
            cout << "Geben Sie die erste Zahl ein: ";
            if (cin >> Zahl_1) {
                break;
            } else {
                cin.clear(); // Fehlerstatus löschen
                cin.ignore(1000, '\n'); // Eingabepuffer leeren
                cout << "Die Eingabe war ungültig. Bitte versuchen Sie es erneut.\n";
                Wiederholungen--;
            }
        }
       // Beendet das Programm, falls die Eingabe 5 mal ungültig war
        if (Wiederholungen == 0) {
            cout << "Die Zahl wurde zu oft falsch eingegeben. Das Programm wird beendet.\n";
            return 1;
        }

        // Zweite Zahl einlesen
        Wiederholungen = 5;
        while (Wiederholungen > 0) {
            cout << "Geben Sie die zweite Zahl ein: ";
            if (cin >> Zahl_2) {
                break;
            } else {
                cin.clear();
                cin.ignore(1000, '\n');
                cout << "Die Eingabe war ungültig. Bitte versuchen Sie es erneut.\n";
                Wiederholungen--;
            }
        }
        if (Wiederholungen == 0) {
            cout << "Die Zahl wurde zu oft falsch eingegeben. Das Programm wird beendet.\n";
            return 1;
        }

        // Operator einlesen
        Wiederholungen = 5;
        while (Wiederholungen > 0) {
            cout << "Geben Sie den Operator ein (+, -, *, /): ";
            cin >> op;
            if (op == '+' || op == '-' || op == '*' || op == '/') {
                break;
            } else {
                cout << "Der Operator war ungültig. Bitte versuchen Sie es erneut.\n";
                Wiederholungen--;
            }
        }
        if (Wiederholungen == 0) {
            cout << "Der Operator wurde zu oft falsch eingegeben. Das Programm wird beendet.\n";
            return 1;
        }

        // Nachricht formatieren und senden
        char message[50];
        snprintf(message, sizeof(message), "%.2f,%.2f,%c", Zahl_1, Zahl_2, op); // String der an Schnittstelle gesendet wird
        write(serialFd, message, strlen(message));
        messageLog.push_back("Ihre Aufgabe lautet: " + string(message)); // Gesendete Nachricht speichern
        usleep(3000000); // kurze Pause, damit der Arduino die Daten verarbeiten kann

        // Antwort der seriellen Schnittstelle empfangen
        char buffer[256] = {0};
        int bytesRead = read(serialFd, buffer, sizeof(buffer) - 1);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0'; // Null-terminieren
            string Antwort = "Das Ergebnis lautet: " + string(buffer);
            cout << Antwort << endl;
            messageLog.push_back(Antwort); // Empfangene Nachricht speichern
        } else {
            cerr << "Fehler beim Lesen von der seriellen Schnittstelle." << endl; // Fehlermeldung bei Problem mit Schnittstelle
        }

        // Serielle Schnittstelle schließen
        close(serialFd);

        // Frage nach weiteren Berechnungen
        int Antwort;
        Wiederholungen = 5;
        while (Wiederholungen > 0) {
            cout << "Möchten Sie eine weitere Berechnung durchführen? Geben Sie 1 für Ja und 2 für Nein ein: ";
            cin >> Antwort;
            if (Antwort == 1) {
                break; // Schleife wird neu gestartet
            } else if (Antwort == 2) {
                goto Berechnung_speichern; // weiter zu Berechnung_speichern
            } else {
                cin.clear();
                cin.ignore(1000, '\n');
                cout << "Die Eingabe war ungültig. Bitte versuchen Sie es erneut.\n";
                Wiederholungen--;
            }
        }
        if (Wiederholungen == 0) {
            cout << "Zu viele ungültige Eingaben. Das Programm wird beendet.\n";
            return 1;
        }
    }
    
// Abfrage ob die Berechnungen gespeichert werden sollen
Berechnung_speichern:
    string Dateiname;
    
    cout << "Möchten Sie ihre Berechnungen speichern? Geben sie 1 für Ja und 2 für Nein ein: ";
    int Antwort_Sicherung;
    cin >> Antwort_Sicherung;
    if (Antwort_Sicherung == 1) {
        cout << "Unter welchem Namen möchten sie die Datei speichern?";
        cin >> Dateiname;
        string Dateipfad = "/Users/juli/Documents/Textdateien_PC_Kommunikation/" + Dateiname + ".txt"; // Dateipfad hier anpassen
        ofstream outFile(Dateipfad);
        // Fehlermeldung bei fehlerhaftem Dateipfad
        if (!outFile) {
            cerr << "Die Datei konnte nicht erstellt werden." << endl;
            return 1;
        }
        // Speichert die Berechnung im angegebnen Dateipfad
        for (const auto &line : messageLog) {
            outFile << line << endl;
        }
        outFile.close();
        cout << "Ihre Berechnungen wurden in '" << Dateipfad << "' gespeichert." << endl;
    }
//    Beendet das Programm, falls keine Speicherung erwünscht
    else {
        cout << "Das Programm wird beendet, ohne die Berechnungen zu speichern." << endl;
    }
    
   cout << "Danke, dass Sie das Programm genutzt haben. Bis bald!\n";  //Verabschiedung

    return 0;
}

