

#ifndef __Jigsaw_HistoryLayer_
#define __Jigsaw_HistoryLayer_

#include "jig_tools.h"
#include "DBRecord.h"
#include "DBMainLevel.h"
#include "cocos-ext.h"

USING_NS_CC_EXT;

class PayResult;

class HistoryLayer : public cocos2d::Layer
{
public:
    JIG_SCENE_FROM_LAYER(HistoryLayer);

    friend class PayResult;
private:
	HistoryLayer();
	virtual ~HistoryLayer() override;

	CREATE_FUNC(HistoryLayer);
	
	virtual bool init() override;

    void initLevel(int level);
    void initPayment();
private:
	cocos2d::Node* load_csd();

	void onClickReturn(Ref* sender);
    void onClickPay(Ref* sender);
	void onClickButton_2(Ref* sender);

    void onClickPageItem(Ref* sender);

    Button* m_btn_pay;
private:
//	ScrollView* m_scroll0;
//	ScrollView* m_scroll1;
//    cocos2d::extension::TableView* m_tb0;
//    cocos2d::extension::TableView* m_tb1;
//	Text* m_title0;
//	Text* m_title1;
    vector<Text*> m_title;
    vector<PageView*> m_pview;
    vector<DBMainLevel> m_level;
    vector<DBRecord> m_record;

    PayResult* m_payResult;
};

#endif

