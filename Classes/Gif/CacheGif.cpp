#include "CacheGif.h"
#include "GIFMovie.h"
#include "GifMovieCache.h"
#include "GifFrameCache.h"

USING_NS_CC;

CacheGif::CacheGif()
{
	m_duration = 0;
	m_progress = 0;
	m_currentIndex = UNINITIALIZED_UINT;
}

CacheGif::~CacheGif()
{
	if(m_frameData.size() > 0)
	for(std::vector<GifSprieFrame*>::iterator iter = m_frameData.begin(); iter != m_frameData.end(); )
	{
		GifSprieFrame* sprite = *iter;
		++iter;

		uint32_t index = sprite->Index();
		std::string gifFrameName = getGifFrameName(index);

		do 
		{
			SpriteFrame* spriteFrame   = sprite->getSpriteFrame();
			CC_BREAK_IF(spriteFrame == NULL);

			bool spriteFrameInCache = SpriteFrameCache::getInstance()->getSpriteFrameByName(gifFrameName.c_str()) == spriteFrame;
            //1. just GifSprieFrame retain
            //2. CCSpriteFrameCache and GifSprieFrame retain
            //more. other gif CacheGif retain
			if(spriteFrame->getReferenceCount() == 1 || (spriteFrame->getReferenceCount() ==2 && spriteFrameInCache))
			{
				Texture2D* texture = sprite->getSpriteFrame()->getTexture();
                Director::getInstance()->getTextureCache()->removeTexture(texture);
				SpriteFrameCache::getInstance()->removeSpriteFramesFromTexture(texture);

			}
		} while (0);

		delete sprite;
	}
}

bool CacheGif::init(const char* fileName)
{
    m_gif_fullpath = fileName;

    bool res = false;
    do
    {
        CC_BREAK_IF(initGifData() == false);
        res = this->initWithSpriteFrame(m_frameData[0]->getSpriteFrame());//init CCSprite with the first frame
    } while (0);

//    if(res && m_frameData.size()>1)
    if(res)
    {
        scheduleUpdate();
    }
    return res;
}

std::string CacheGif::getGifFrameName(int index)
{
	const char* frame = CCString::createWithFormat("%s_%d",m_gif_fullpath.c_str(), index)->getCString();
	return frame;
}

// get CCSpriteFrame from cache or create with Bitmap's data
SpriteFrame* CacheGif::getGifSpriteFrame(Bitmap* bm, int index)
{
	std::string gifFrameName = getGifFrameName(index);
	SpriteFrame* spriteFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName(gifFrameName.c_str());
	if(spriteFrame != NULL)
	{
		return spriteFrame;
	}
	
	do 
	{	
		Texture2D* texture = createTexture(bm,index,true);
		CC_BREAK_IF(! texture);

		spriteFrame = SpriteFrame::createWithTexture(texture, Rect(0,0,texture->getContentSize().width, texture->getContentSize().height));
		CC_BREAK_IF(! spriteFrame);

		SpriteFrameCache::getInstance()->addSpriteFrame(spriteFrame, gifFrameName.c_str());
	} while (0);


	return spriteFrame;
}

bool CacheGif::initGifData()
{
//    std::vector<GifFrame> frames = GifFrameCache::getInstance()->getGifFrames(m_gif_fullpath);
//    for (auto it = frames.begin(); it != frames.end(); ++it)
//    {
//        if (it->m_bm==nullptr || !it->m_bm->isValid()) {
//            continue;
//        }
//
//        addGifSpriteFrame(*it);
//    }
//
//    return (m_frameData.size() > 0);

    GIFMovie* movie = GifMovieCache::getInstance()->getMovie(m_gif_fullpath);
    lazyInitFrame();
    CallFunc* load = CallFunc::create( std::bind(&CacheGif::lazyInitFrame, this) );
    Repeat* repeat = Repeat::create(load, movie->getGifCount());
    this->runAction(repeat);
    return m_frameData.size()>0;
}

void CacheGif::lazyInitFrame()
{
    GIFMovie* movie = GifMovieCache::getInstance()->getMovie(m_gif_fullpath);
    if (m_frameData.size()<movie->getGifCount())
    {
        addGifSpriteFrame(movie->getGifFrameByIndex( m_frameData.size() ));
    }
}

void CacheGif::updateGif(uint32_t delta)
{
	m_progress += delta;

	if(m_progress > m_duration)
	{
		m_progress = m_progress - m_duration;
	}

	uint32_t dur = 0;
	for (int i = 0; i < m_frameData.size(); i++)
	{
		dur += m_frameData[i]->Duration();
		if (dur >= m_progress)// If the progress bigger than sum of pre frame duration，change new CCSpriteFrame to display
		{
			if(m_frameData[i]->Index() != m_currentIndex)
			{
				this->setDisplayFrame(m_frameData[i]->getSpriteFrame());
			}
			m_currentIndex = m_frameData[i]->Index();

			return ;
		}
	}
}

void CacheGif::addGifSpriteFrame(const GifFrame& gifFrame)
{
    if(gifFrame.m_frameData.m_index == UNINITIALIZED_UINT)
    {
        return;
    }

    //正常情况下，没什么用。
//	if(m_frameData.size() > 0)
//	{
//		for(std::vector<GifSprieFrame*>::iterator iter = m_frameData.begin(); iter != m_frameData.end(); iter++)
//		{
//			if((*iter)->Index() == gifFrame.m_frameData.m_index)
//			{
//				return ;
//			}
//		}
//	}

	cocos2d::SpriteFrame* spriteFrame = getGifSpriteFrame(gifFrame.m_bm, gifFrame.m_frameData.m_index);
    
	GifSprieFrame* frame = new GifSprieFrame;
	frame->setFrameData(gifFrame.m_frameData);
	frame->setSpriteFrame(spriteFrame);
	m_frameData.push_back(frame);
    
	m_duration += frame->Duration();
}

void GifSprieFrame::setSpriteFrame(cocos2d::SpriteFrame* frame)
{
	CC_SAFE_RETAIN(frame);
	CC_SAFE_RELEASE(m_frame);
	m_frame = frame;
}