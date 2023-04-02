#pragma once

#include "LinkedList.h"
#include "section.h"
#include "selector.h"
#include "attribute.h"
#include "str.h"

#include <iostream>

using std::cout, std::endl;

class BlockStorage
{
	const int max_capacity_ = 8;
	unsigned int section_count_ = 0;

	LinkedList<LinkedList<section>> sections_;

public:
	BlockStorage() = default;

	[[nodiscard]] LinkedList<LinkedList<section>> get_data() const { return sections_; }

	void add_new_element(const section& new_section)
	{
		if (sections_.length() == 0 || sections_.back().length() == max_capacity_)
		{
			LinkedList<section> new_linked_list;

			new_linked_list.push_back(section(new_section));
			sections_.push_back(LinkedList(new_linked_list));
		} else
		{
			auto& last_block = sections_.back();
			last_block.push_back(section(new_section));
		}
		this->section_count_++;
	}

	[[nodiscard]] section& find_section(const int index) const
	{
		int section_index = 0;
		for (auto& block : sections_)
		{
			for (auto& element : block)
			{
				if (section_index++ == index) return element;
			}
		}
		section zero;
		return zero;
	}

	//Commands

	//?
	void display_sections_count() const
	{
		cout << "? == " << this->section_count_ << endl;
	}

	//i,S,?
	void display_selectors_for_section(const int i) const
	{
		if (i >= this->section_count_) return;
		const auto section = find_section(i);

		const auto& result = section.get_selectors();

		LinkedList<selector> new_selectors;

		for (const auto& sel : result)
		{
			bool found_duplicate = false;
			for (const auto& new_sel : new_selectors)
			{
				if (sel.get_name() == new_sel.get_name())
				{
					found_duplicate = true;
					break;
				}
			}

			if (!found_duplicate) new_selectors.push_back(sel);
		}

		cout << i + 1 << ",S,? == " << new_selectors.length() << endl;
	}

	//i,A,?
	void display_attribute_count_for_section(const int i) const
	{
		if (i >= this->section_count_) return;
		const auto section = find_section(i);

		const auto& attributes = section.get_attributes();

		LinkedList<attribute> new_attributes;

		for (const auto& attr : attributes)
		{
			bool found_duplicate = false;
			for (const auto& new_attr : new_attributes) {
				if (attr.get_name() == new_attr.get_name()) {
					found_duplicate = true;
					break;
				}
			}
			if (!found_duplicate) {
				new_attributes.push_back(attr);
			}
		}

		cout << i + 1 << ",A,? == " << new_attributes.length() << endl;
	}


	//i,S,j
	void selector_for_block(const int i, const int j) const
	{
		if (i >= this->section_count_) return;
		const auto section = find_section(i);
		if (j >= section.get_selectors().length()) return;

		const auto& result = section.get_selectors().get(j).get_name();

		cout << i + 1 << ",S," << j + 1 << " == " << result.c_str() << endl;
	}


	//i,A,n
	void attribute_name_value(const str& n_name, const int i) const
	{
		const auto section = find_section(i);

		const auto& attrs = section.get_attributes();

		for (auto it = attrs.rbegin(); it != attrs.rend(); ++it)
		{
			const auto& curr_attr = *it;
			if (curr_attr.get_name() == n_name)
			{
				cout << i + 1 << ",A," << n_name << " == " << curr_attr.get_value() << endl;
				break;
			}
		}
	}

	//n,A,?
	void count_attribute_occ(const str& n_name) const
	{
		int count = 0;
		for (const auto& block : sections_)
		{
			const auto& current_block = block;
			for (const auto& section : current_block)
			{
				const auto& current_attrs = section.get_attributes();
				for (const auto& attr : current_attrs)
				{
					if (attr.get_name() == n_name)
					{
						count++;
						break;
					}
				}
			}
		}
		cout << n_name << ",A,? == " << count << endl;
	}

	//z,S,?
	void count_selector_occ(const str& n_name) const
	{
		int count = 0;
		for (const auto& block : sections_)
		{
			const auto& current_block = block;
			for (const auto& section : current_block)
			{
				const auto& current_selectors = section.get_selectors();
				for (const auto& sel : current_selectors)
				{
					if (sel.get_name() == n_name)
					{
						count++;
						break;
					}
				}
			}
		}
		cout << n_name << ",S,? == " << count << endl;
	}

	//z,E,n
	void z_e_n(const str& n_name, const str& z_name) const
	{
		for (auto b_it = sections_.rbegin(); b_it != sections_.rend(); ++b_it)
		{
			const auto& curr_block = *b_it;
			for (auto s_it = curr_block.rbegin(); s_it != curr_block.rend(); ++s_it)
			{
				const auto& selectors = (*s_it).get_selectors();
				for (auto a_it = selectors.rbegin(); a_it != selectors.rend(); ++a_it)
				{
					const auto& curr_selector = *a_it;
					if (curr_selector.get_name() == z_name)
					{
						for (const auto& attr : (*s_it).get_attributes())
						{
							if (attr.get_name() == n_name)
							{
								cout << z_name << ",E," << n_name << " == " << attr.get_value() << endl;
								return;
							}
						}
					}
				}
			}
		}
	}

	//i,D,*
	void delete_entire_section(const int i)
	{
		if (i >= this->section_count_ || i < 0) return;

		int current_index = 0;
		auto list_it = sections_.begin();
		while (list_it != sections_.end() && current_index + list_it->length() <= i) {
			current_index += list_it->length();
			++list_it;
		}

		int inner_index = i - current_index;
		auto node_it = list_it->begin();
		while (inner_index > 0) {
			++node_it;
			--inner_index;
		}

		list_it->erase(node_it);
		--section_count_;

		cout << i + 1 << ",D,* == deleted" << endl;
	}

	void print_storage()
	{
		for (int i = 0; i < sections_.length(); i++)
		{
			cout << "Content of block " << i << ": " << endl;
			for (const auto& element : sections_.get(i))
			{
				for (const auto& sel : element.get_selectors())
				{
					cout << sel.get_name() << " ";
				}
				cout << "{" << endl;
				for (const auto& attr : element.get_attributes())
				{
					cout << "\t" << attr.get_name() << ": " << attr.get_value() << ";" << endl;
				}
				cout << "}" << endl;
			}
			cout << "==============================" << endl;
		}
	}

	//i,D,n
	void delete_attribute(const int i, const str& n_name)
	{
		if (i >= this->section_count_ || i < 0) return;

		auto current_index = 0;
		auto sections_it = sections_.begin();

		while (sections_it != sections_.end() && current_index + sections_it->length() <= i)
		{
			current_index += sections_it->length();
			++sections_it;
		}

		int inner_index = i - current_index;
		auto node_it = sections_it->begin();
		while (inner_index > 0)
		{
			++node_it;
			--inner_index;
		}

		auto& attributes = node_it->get_attributes();
		auto a_it = attributes.begin();
		while (a_it != attributes.end())
		{
			if (a_it->get_name() == n_name)
			{
				attributes.erase(a_it);
				cout << i + 1 << ",D," << n_name << " == deleted" << endl;
				break;
			}
			++a_it;
		}

		if (node_it->get_attributes().empty())
		{
			sections_it->erase(node_it);
			--section_count_;
		}
	}
};