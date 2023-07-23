class CTcrCompressor {
private: //Data
	iInStream, iOutStream;
	iBuffer;
	iDictionary;
private: //Procedures
	void ReadInputFile();
	void WriteOutputFile();
	void InitDictionary();
	TBool Merge();
	TBool DeleteUnused();
	TBool AddNew();
public:
	CTcrCompressor();
	Compress();
}