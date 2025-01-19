// Taschenrechner passend zu xCode Programm "Portfolio C++"
// führt Berechnungen aufgrund der Operators durch
// Serielle Eingabe muss im Format "Zahl1, Zahl2, Operator" erfolgen
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 50 // Maximale Länge des Eingabestrings

char Aufgabe[BUFFER_SIZE]; // Speicher für den Eingabestring

void setup(void) {
    Serial.begin(9600); // Bauderate auf 9600 festlegen
}

void loop(void) {
    static bool Eingangssignal = false; // noch kein Eingangssignal vorhanden

    
    
    
    if (Serial.available() > 0 && !Eingangssignal) { // prüfen ob Daten an der seriellen Schnittstelle verfügbar sind
        int length = Serial.readBytesUntil('\n', Aufgabe, BUFFER_SIZE - 1); // serielle Daten auslesen
        Aufgabe[length] = '\0'; // Null-terminiere den String
        Eingangssignal = true; // Einganssignal vorhanden
    }
    

    if (Eingangssignal) {

        // Datentype festlegen
        char *token;
        const char *Trennzeichen = ","; 
        double Zahl1, Zahl2, Ergebnis;
        char Operator;

        // Herauslesen und speichern der ersten (Zahl1)
        token = strtok(Aufgabe, Trennzeichen);
        if (token != NULL) {
            Zahl1 = atof(token); // Konvertiere Zahl1 zum Datentyp double
            
        }

        // Herauslesen und speichern der zweiten Zahl (Zahl2)
        token = strtok(NULL, Trennzeichen);
        if (token != NULL) {
            Zahl2 = atof(token); // Konvertiere Zahl2 zum Datentyp double
        }

        // Herauslesen und speichern des Operators (Operator)
        token = strtok(NULL, Trennzeichen);
        if (token != NULL) {
            Operator = token[0]; // Nimmt das erste Zeichen des Tokens
        }

        // Berechnung des Ergebnises basierend auf dem Operator
        // Fehlermeldung falls durch Null dividiert wird
        switch (Operator) {
            case '+':
                Ergebnis = Zahl1 + Zahl2;
                break;
            case '-':
                Ergebnis = Zahl1 - Zahl2;
                break;
            case '*':
                Ergebnis = Zahl1 * Zahl2;
                break;
            case '/':
                if (Zahl2 != 0) {
                    Ergebnis = Zahl1 / Zahl2;
        } else {
                    Serial.println("Fehler: Eine Division durch Null ist nicht möglich!");
                    Eingangssignal = false;
                    return;
                }
              
        }

        // Ausgabe des Ergebnisses
      Serial.print(Ergebnis);
      

        Eingangssignal = false; // Eingangssignal rücksetzen, um weitere Eingaben zu ermöglichen
        
    }
    }

    
