#include <algorithm>
#include <iostream>
#include "../includes/Labels.hpp"

using Abstract::Labels;

std::string Labels::GetLabel()
{ // When requesting singular Label, likely want the latest one.
	if (labels_.empty())
		return "";
	return labels_.back();
}


bool Labels::CompareLabels(const std::vector<std::string> otherLabels)
{ // If any label here matches any in other label, return true
	for(auto &otherLabel : otherLabels)
	{
		if (std::find(labels_.begin(), labels_.end(), otherLabel ) != labels_.end() )
		{
			//std::cout << "Node labels match for " << this->GetLabel() << " & " << otherLabel << "\n";
			return true;
		}
		//std::cout << "Node labels DONT match for " << this->GetLabel() << " & " << otherLabel << "\n";
	}
	return false;
}
