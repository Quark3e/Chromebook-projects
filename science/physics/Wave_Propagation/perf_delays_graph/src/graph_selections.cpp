
#include "graph_selections.hpp"


DIY::typed_dict<std::string, void (*)()> graph_selections({
    {"Default", graph__default},
    {"Total time", graph__totalTime},
    {"processTime", graph__processTime},
    {"iterationDelays", graph__iterationDelays}
});


void graph__default() {

}
void graph__totalTime() {

}
void graph__processTime() {

}
void graph__iterationDelays() {

}
