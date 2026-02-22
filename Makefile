all: data_aggregator_analyzer.exe

data_aggregator_analyzer.exe: data_aggregator_analyzer.cpp
	g++ -std=c++20 -o data_aggregator_analyzer.exe data_aggregator_analyzer.cpp -I ../openai-cpp/include/openai -lcurl

clean:
	rm -f *.exe	


	
