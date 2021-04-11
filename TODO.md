# TODO LIST

* Separate beatPlayer logic to a separate static library project. The app simply serves as a wrapper to play and test with it
* beatPlayer logic:
  - It's vital that the beatPlayer logic only maintains the logic it absolutely needs.
  - assets can be dynamically loaded to the object, although only one midi file can be assigned at a time
  - active waveforms don't override each other. They delete themselves at runtime upon reaching the end of each audio snippet.
  - Make use of more midi note information, mainly the amplitude.