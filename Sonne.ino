void SunUpDown (void) {
  if (day() != PreDay) {
    //PreDay = day();
    Heute[0] = 0; Heute[1] = 0; Heute[2] = 12; Heute[3] = day(); Heute[4] = month(); Heute[5] = year() - 2000; // store today's date (at noon) in an array for TimeLord to use
    int JetztZeit = hour() * 60 + minute();
    int SonneAufZeit, SonneUnterZeit;
    if (City.SunRise(Heute)) {
      if (LTZ.locIsDST(now())) {
        Heute[tl_hour] += 1;
      }
      Serial.print("  Sonnenaufgang heute um ");
      Serial.print(Heute[tl_hour]);
      Serial.print(":");
      printDigits(Heute[tl_minute]);
      Serial.println();
      SonneAufZeit = Heute[tl_hour] * 60 + Heute[tl_minute];
    }
    if (City.SunSet(Heute)) {
      if (LTZ.locIsDST(now())) {
        Heute[tl_hour] += 1;
      }
      Serial.print("Sonnenuntergang heute um ");
      Serial.print(Heute[tl_hour]);
      Serial.print(":");
      printDigits(Heute[tl_minute]);
      SonneUnterZeit = Heute[tl_hour] * 60 + Heute[tl_minute];
    }
    if ((JetztZeit > SonneAufZeit) && (JetztZeit < SonneUnterZeit)) {
      SonneDa = true;
      PreSonneDa = SonneDa;
      Serial.println("\nDie Sonne ist da");
    }
    else if ((JetztZeit < SonneAufZeit) || (JetztZeit > SonneUnterZeit)) {
      SonneDa = false;
      PreSonneDa = SonneDa;
      Serial.println("\nDie Sonne ist weg");
    }
  }
}
