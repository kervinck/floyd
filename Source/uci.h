
#if !defined(floydVersion)
 #define floydVersion <invalid>
#endif

bool uciSearchInfo(void *uciInfoData);
void uciMain(Engine_t engine);

double uciBenchmark(Engine_t self, double time, searchInfo_fn *infoFunction, void *infoData);

