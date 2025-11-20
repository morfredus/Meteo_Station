// utils.h - utilitaires
#pragma once
#include <Arduino.h>

String removeAccents(String str) {
  str.replace("à", "a"); str.replace("À", "A"); 
  str.replace("é", "e"); str.replace("É", "E"); 
  str.replace("è", "e"); str.replace("È", "E"); 
  str.replace("ê", "e"); str.replace("Ê", "E"); 
  str.replace("ë", "e"); str.replace("Ë", "E"); 
  str.replace("ç", "c"); str.replace("Ç", "C"); 
  str.replace("ù", "u"); str.replace("Ù", "U"); 
  str.replace("û", "u"); str.replace("Û", "U"); 
  str.replace("ü", "u"); str.replace("Ü", "U"); 
  str.replace("î", "i"); str.replace("Î", "I"); 
  str.replace("ï", "i"); str.replace("Ï", "I"); 
  str.replace("ô", "o"); str.replace("Ô", "O"); 
  str.replace("œ", "oe"); str.replace("Œ", "OE"); 
  str.replace("ä", "a"); str.replace("Ä", "A"); 
  return str;
}