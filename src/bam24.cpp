#include "bam24.hpp"
#include <iostream>
#include <tuple>
#include <vector>
#include <utility>

#include "SamFile.h"
#include "SamFlag.h"
#include "SamValidation.h"

using namespace std;


/**
 * This function reads a BAM/SAM file and maps the read contained
 * within. The resulting datatype is pos -> [(readID, Nuc)] in SML
 * notation. Basically, its a vector with the length of the reference
 * sequence. At each position there is a list of all read which have
 * a nucleotide mapping to that position.
 */
mappedReads_t* bam24( char * filename){
	SamFile file;
	SamFileHeader header;

	if( !file.OpenForRead(filename) || !file.ReadHeader(header) ){
		return NULL;
	}

	// Get the Reference Sequence (SQ)
	SamHeaderRecord *header_record = header.getNextSQRecord();
	size_t length = atoi(header_record->getTagValue("LN"));

	auto ret = new mappedReads_t(length, std::list<seqNuc_t>());

	SamRecord record, next_record;

	size_t counter = 0;
	// Get the next read.
	while( file.ReadRecord(header, record)){
		size_t seqID = counter++;

		size_t start = record.get0BasedPosition();
		size_t length = record.getReadLength();

		// Our simulations does not contain paired and reads. So the following
		// code is not yet checked for correctness.
		/* if( SamFlag::isProperPair( record.getFlag())){
			file.ReadRecord( header, next_record);
			int id2 = next_record.getReferenceID();

			auto start1 = next_record.get0BasedPosition();
			auto end1 = next_record.get0BasedAlignmentEnd();

			for( size_t i= 0; end1 > start1 && i< size_t(end1 - start1); i++){
				cout << i << endl;
				int inner = next_record.getCigarInfo()->getQueryIndex(i);

				if( inner > 0){
					if( (*ret)[inner] == NULL){
						(*ret)[inner] = new vector<entry>();
					}
					entry p = make_pair(id2, next_record.getSequence(inner));
					(*ret)[inner]->push_back(p);
				}
			}
		} */

		// iterate over the read.
		for( size_t i = 0; i < length; i++){
			int pos_in_ref = i + start;//record.getCigarInfo()->getRefPosition(i, start);

			// add the current nucleotide to the map.
			seqNuc_t p = make_pair(seqID, record.getSequence(i));
			(*ret)[pos_in_ref].push_back(p);
		}
	}

	file.Close();

	return ret;
}
