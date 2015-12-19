/*
* This is a heavily modified fork of gumbo-query by Hoping White aka LazyTiger.
* The original software can be found at: https://github.com/lazytiger/gumbo-query
*
* gumbo-query is based on cascadia, written by Andy Balholm.
*
* Copyright (c) 2011 Andy Balholm. All rights reserved.
* Copyright (c) 2015 Hoping White aka LazyTiger (hoping@baimashi.com)
* Copyright (c) 2015 Jesse Nicholson
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/

#include "GQUnarySelectory.hpp"
#include "GQNode.hpp"

namespace gq
{

	GQUnarySelectory::GQUnarySelectory(SelectorOperator op, SharedGQSelector selector) :
		m_operator(op), m_selector(std::move(selector))
	{
		if (m_selector == nullptr)
		{
			throw std::runtime_error(u8"In GQUnarySelectory::GQUnarySelectory(SelectorOperator, SharedGQSelector) - Supplied shared selector is nullptr.");
		}

		#ifndef NDEBUG
			#ifdef GQ_VERBOSE_DEBUG_NFO
			std::cout << "Built GQUnarySelectory with operator " << static_cast<size_t>(m_operator) << u8"." << std::endl;
			#endif
		#endif

		// Take on the traits of the selector.
		auto& t = m_selector->GetMatchTraits();
		for (auto& tp : t)
		{
			AddMatchTrait(tp.first, tp.second);
		}
	}

	GQUnarySelectory::~GQUnarySelectory()
	{
	}

	const bool GQUnarySelectory::GQUnarySelectory::Match(const GQNode* node) const
	{

		// If it's not a not selector, and there's no children, we can't do a child or descendant match
		if (m_operator != SelectorOperator::Not && node->GetNumChildren() == 0)
		{
			return false;
		}

		switch (m_operator)
		{
			case SelectorOperator::Not:
			{
				return m_selector->Match(node) == false;
			}
			break;

			case SelectorOperator::HasDescendant:
			{
				return HasDescendantMatch(node);
			}
			break;

			case SelectorOperator::HasChild:
			{
				auto nNumChildren = node->GetNumChildren();
			
				for (size_t i = 0; i < nNumChildren; i++)
				{
					auto child = node->GetChildAt(i);

					if (m_selector->Match(child.get()))
					{
						return true;
					}
				}

				return false;				
			}
			break;	
		}

		// Just to shut up the compiler.
		return false;
	}

	const bool GQUnarySelectory::HasDescendantMatch(const GQNode* node) const
	{
		for (size_t i = 0; i < node->GetNumChildren(); i++)
		{
			auto child = node->GetChildAt(i);

			if (m_selector->Match(child.get()) || HasDescendantMatch(child.get()))
			{
				return true;
			}
		}

		return false;
	}

} /* namespace gq */
