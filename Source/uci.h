
bool uciSearchInfo(void *uciInfoData, const char *string, ...);
void uciMain(Engine_t engine);

double uciBenchmark(Engine_t self, double time, searchInfo_fn *infoFunction, void *infoData);

