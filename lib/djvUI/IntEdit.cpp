// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/IntEdit.h>

#include <djvUI/IntLabel.h>

#include <djvCore/NumericValueModels.h>
#include <djvCore/ValueObserver.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct IntEdit::Private
        {
            std::shared_ptr<ValueObserver<IntRange> > rangeObserver;
            std::shared_ptr<ValueObserver<int> > valueObserver;
        };

        void IntEdit::_init(const std::shared_ptr<Context>& context)
        {
            NumericEdit::_init(context);
            setClassName("djv::UI::IntEdit");
            setModel(IntValueModel::create());
        }

        IntEdit::IntEdit() :
            _p(new Private)
        {}

        IntEdit::~IntEdit()
        {}

        std::shared_ptr<IntEdit> IntEdit::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<IntEdit>(new IntEdit);
            out->_init(context);
            return out;
        }

        void IntEdit::setModel(const std::shared_ptr<INumericValueModel<int> > & model)
        {
            INumericEdit<int>::setModel(model);
            DJV_PRIVATE_PTR();
            if (model)
            {
                auto weak = std::weak_ptr<IntEdit>(std::dynamic_pointer_cast<IntEdit>(shared_from_this()));
                p.rangeObserver = ValueObserver<IntRange>::create(
                    model->observeRange(),
                    [weak](const IntRange&)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_textUpdate();
                    }
                });
                p.valueObserver = ValueObserver<int>::create(
                    model->observeValue(),
                    [weak](int)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_textUpdate();
                    }
                });
            }
            else
            {
                p.rangeObserver.reset();
                p.valueObserver.reset();
            }
        }

        void IntEdit::_setIsMin(bool value)
        {
            NumericEdit::_setIsMin(value);
        }

        void IntEdit::_setIsMax(bool value)
        {
            NumericEdit::_setIsMax(value);
        }

        void IntEdit::_textEdit(const std::string& value, TextEditReason reason)
        {
            if (auto model = getModel())
            {
                try
                {
                    model->setValue(std::stoi(value));
                }
                catch (const std::exception& e)
                {
                    _log(e.what(), LogLevel::Error);
                }
                _textUpdate();
                _doCallback(reason);
            }
        }

        bool IntEdit::_keyPress(int value)
        {
            return INumericEdit<int>::_doKeyPress(fromGLFWKey(value));
        }

        void IntEdit::_scroll(float value)
        {
            if (auto model = getModel())
            {
                model->setValue(model->observeValue()->get() + static_cast<int>(model->observeSmallIncrement()->get() * value));
            }
        }

        void IntEdit::_incrementValue()
        {
            if (auto model = getModel())
            {
                model->incrementSmall();
            }
        }

        void IntEdit::_decrementValue()
        {
            if (auto model = getModel())
            {
                model->decrementSmall();
            }
        }

        void IntEdit::_textUpdate()
        {
            if (auto model = getModel())
            {
                std::stringstream ss;
                ss << model->observeValue()->get();
                NumericEdit::_textUpdate(ss.str(), IntLabel::getSizeString(model->observeRange()->get()));
            }
            _resize();
        }

    } // namespace UI
} // namespace djv
