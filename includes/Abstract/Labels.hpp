#ifndef LABELS_HPP
#define LABELS_HPP

#include <string>
#include <vector>

namespace Abstract
{
	class Labels
	{
	public:
		//////////////////////////////////////////////////////////
		//                       CONSTRUCTOR                    //
		//////////////////////////////////////////////////////////
		Labels() {};
		Labels(std::string label) {this->AddLabel(label);}
		Labels(std::vector<std::string> labels) {this->SetLabels(labels);}
		//////////////////////////////////////////////////////////
		//                       ACCESSOR                       //
		//////////////////////////////////////////////////////////
		inline std::vector<std::string> GetLabels() {return labels_;}
		std::string GetLabel();
		//////////////////////////////////////////////////////////
        //                       MUTATOR                        //
        //////////////////////////////////////////////////////////
		inline void SetLabels(std::vector<std::string> labels) {labels_ = labels;}
		inline void AddLabel(std::string label) {labels_.push_back(label);}
		//////////////////////////////////////////////////////////
        //                      FUNCTIONS                       //
        //////////////////////////////////////////////////////////
		bool CompareLabels(const std::vector<std::string> otherLabels);
		//////////////////////////////////////////////////////////
        //                  OPERATOR OVERLOADING                //
        //////////////////////////////////////////////////////////
        // bool operator== (const Node<T>& rhs) const { return (this->GetIndex() == rhs.GetIndex());}
        // bool operator!= (const Node<T>& rhs) const { return (this->GetIndex() != rhs.GetIndex());}
 		
	private:
		//////////////////////////////////////////////////////////
        //                       FUNCTIONS                      //
        //////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////
        //                       ATTRIBUTES                     //
        //////////////////////////////////////////////////////////
		std::vector<std::string> labels_;
		
	};
}
#endif // LABELS_HPP