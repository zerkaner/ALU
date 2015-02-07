#pragma once

/** The Perception Unit (PU) is a container responsible for querying the attached
 *  sensors and storing the retrieved results for further usage in the planning phase. */
class PerceptionUnit {

  /* Here follows a listing of sensors and input storages. It is distinguished between
   * data sensors (they get an object from a data source) and channel-based communication
   * sensors, who listen on a channel and report all present messages to this unit, 
   * also wrapped in a SensorInput. The integer represent information type or channel. */

  //TODO TBC.


  public:
    
    /** Request new sensor information from all attached sensors and store them in a map. */
    void SenseAll() {
      // ...
    }

};
