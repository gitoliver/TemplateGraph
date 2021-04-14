#ifndef LABELS_HPP
#define LABELS_HPP

#include <string>
#include <vector>
#include <algorithm>

//TODO: Pick a better namespace name for this, figure out what we should use instead of `unsigned int`
namespace Abstract
{
class Labels
{
public:
	/************************************************
	 *  CONSTRUCTORS/DESTRUCTORS
	 ***********************************************/
	Labels();
	Labels(std::string name);
	Labels(std::vector<std::string> labels);
	Labels(std::string name, std::string label);
	Labels(std::string name, std::vector<std::string> labels);
	~Labels();

	/************************************************
	 *  GETTER/SETTER
	 ***********************************************/
	std::string getName() const;
	std::string getLabel() const;
	std::vector<std::string> getLabels() const;

	void setName(std::string name);
	void setLabels(std::string label);
	void setLabels(std::vector<std::string> labels);

	/************************************************
	 *  MUTATORS
	 ***********************************************/
	void addLabel(std::string label);
	void addLabels(std::vector<std::string> labels);

	/************************************************
	 *  FUNCTIONS
	 ***********************************************/
	bool containsLabel(const std::string query);
	bool compareLabels(const std::vector<std::string> otherLabels);
	bool compareLabels(const std::vector<std::string> otherLabels,
			unsigned int numMatches);

private:
	/************************************************
	 *  ATTRIBUTES
	 ***********************************************/
	std::string name;
	std::vector<std::string> labels;

};
}

#endif // LABELS_HPP
