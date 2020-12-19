char * directionName(Direction dir) {
  char * directionNames[] = {
                             "RIGHT",
                             "UP",
                             "LEFT",
                             "DOWN",
                             "HORIZONTAL",
                             "VERTICAL",
                             "STATIONARY",
                             "RANDOMDIR",
                             "RANDOM",
                             "REL_UP",
                             "REL_DOWN",
                             "REL_LEFT",
                             "REL_RIGHT",
                             "USE",
                             "NONE",
                             "COND_NO",
                             "QUIT",
                             "RESTART",
                             "UNDO",
                             "UNSPECIFIED"
  };
  return directionNames[dir];
}
