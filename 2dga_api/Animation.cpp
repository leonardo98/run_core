#include <algorithm>
#include "Animation.h"
#include "../Core/Render.h"
#include "../Core/Core.h"
#include "../Core/Math.h"

#include <set>
#include <assert.h>

std::set<std::string> _files;

namespace My {

void Animation::AddBone(MovingPart *bone) {
    _renderList.push_back(bone);
}

//
// Деструктор - удаляем все подвижные спрайты
// (текстуры выгружает только менеджер анимаций, 
// т.к. на одной текстуре может быть несколько разных анимаций)
//
Animation::~Animation() {
	for (unsigned int i = 0; i < _bones.size(); ++i) {
		delete _bones[i];
	}
}

//
// Конструктор - параметры анимации читаются из XML 
// можно использовать любой парсер,
// который умеет перебирать ноды и читать атрибуты,
// здесь успользован TinyXml
//
Animation::Animation(const std::string &id, rapidxml::xml_node<> *xe, GLTexture *hTexture, float width, float height)
	: animationName(id)
{
	_simple = true;
	_time = fatof(xe->first_attribute("time")->value());
	_pivotPos.x = fatof(xe->first_attribute("pivotX")->value());
	_pivotPos.y = fatof(xe->first_attribute("pivotY")->value());
	if (hTexture)
	{
        assert(hTexture->Width() > 0 && hTexture->Height() > 0);
	}
	else
	{
		assert(width > 0 && height > 0);
	}

	rapidxml::xml_node<> *element = xe->first_node();
	_bones.reserve(20);
	while (element) {
		_bones.push_back(new MovingPart(this, element, hTexture, width, height));
		element = element->next_sibling();
	}
	_subPosition.Unit();
	_subPosition.Move(-_pivotPos.x, -_pivotPos.y);

	std::sort(_renderList.begin(), _renderList.end(), CmpBoneOrder);

	_vb.tex = hTexture;
	_vb.Reserve(_renderList.size() * 4, _renderList.size() * 6);
}

void Animation::DetachAll()
{
	for(unsigned int i = 0; i < _renderList.size();i++) {
		_renderList[i]->DetachAll();
	}
}

//
// Нарисовать анимацию на экране 
// "position" - значение времени для анимации из диапазона [0, 1]
//
void Animation::Draw(float position) 
{
	assert(0 <= position && position <= 1.f);
	for(unsigned int i = 0; i < _renderList.size();i++) {
		_renderList[i]->_visible = false;
	}

	// для задания положения все спрайтов использую свой внутренний "стек" матриц
	Matrix matrix(_subPosition);
	for(unsigned int i = 0; i < _bones.size();i++) {
		_bones[i]->PreDraw(position, matrix);
	}    

	if (_simple)
	{
		_vb.Init();
		for(unsigned int i = 0; i < _renderList.size();i++) 
		{
			MovingPart &p = *_renderList[i];
			if (p._visible && p._hasTexture && p._enable) {
				Render::PushColorAndMul(static_cast<unsigned int>(0xFF * p._alphaCurrent) << 24 | 0xFFFFFF);
				unsigned int color = Render::GetColor();
				Render::PopColor();
                _vb.AddQuad(p._quad, color);
			}
		}
        Render::DrawVertexBuffer(_vb);
	}
	else
	{
		for(unsigned int i = 0; i < _renderList.size();i++) {
			_renderList[i]->Draw();
		}
		for(unsigned int i = 0; i < _renderList.size();i++) {
			_renderList[i]->SecondLayerDraw();
		}
	}
}

void Animation::ReloadTexture(GLTexture *hTexture) 
{ 
	for (unsigned int i = 0; i < _bones.size(); ++i)
	{
		_bones[i]->ReloadTexture(hTexture);
	}
	_vb.tex = hTexture;
}


//
// Время полного цикла анмиации 
// эту функцию нужно использовать в Update для расчета 
// значения "position" метода Draw
// например, для зацикленой анимации можно считать так:
// progress += deltaTime / anim->Time();
// if(progress >= 1.f) progress = 0.f;
//
float Animation::Time() const {
	return _time;
}

bool Animation::PixelCheck(const FPoint2D &pos) {
	for (My::List::iterator i = _renderList.begin(), e = _renderList.end(); i != e; ++i) {
		if ((*i)->PixelCheck(pos)) {
			return true;
		}
	}
	return false;
}

MovingPart * Animation::GetMovingPart(const std::string &boneName) 
{
	_simple = false;
	for(unsigned int i = 0; i < _renderList.size();i++) {
		if (_renderList[i]->boneName == boneName) {
			return _renderList[i];
		}
	}
	return NULL;
}


// делаем "слепок" анимации без учета установленой матрицы
bool AnimationState::InitWithAnimation(Animation *anim, float position, Animation *target) {
	if (anim->_renderList.size() != target->_renderList.size())
	{
		return false;
	}
	assert(anim->_renderList.size() == target->_renderList.size());
	std::map<std::string, PartPosition> bones;
	for (unsigned int i = 0; i < anim->_renderList.size(); ++i) {
		anim->_renderList[i]->PreCalcLocalPosition(position);
		bones[anim->_renderList[i]->boneName] = anim->_renderList[i]->precalculatedLocalPosition;
	}
	_partPositions.resize(target->_renderList.size());
	for (unsigned int i = 0; i < target->_renderList.size(); ++i) {

		const std::string &boneName = (target->_renderList[i]->boneName);
		if (bones.find(boneName) == bones.end())
		{
			return false;
		}
		assert(bones.find(boneName) != bones.end() && "в анимациях разные имена у костей");
		// в анимациях разные имена у костей

		_partPositions[i] = bones[boneName];
		_partPositions[i].name = boneName;

		assert(fabs(_partPositions[i].angle) < 10 && "что-то не проиницилизировалось");
		// что-то не проиницилизировалось
	}
	return true;
}

// делаем "слепок" анимации без учета установленой матрицы
//void AnimationState::InitWithAnimations(Animation *animOne, float positionOne, Animation *animTwo, float positionTwo, float mix) {
//	_partPositions.resize(animOne->_renderList.size());
//
//	float minus = 1 - mix;
//	for(unsigned int i = 0; i < animOne->_renderList.size();i++) {
//		MovingPart *m = animOne->_renderList[i];
//		m->_visible = false;
//		m->PreCalcLocalPosition(positionOne);
//
//		MovingPart *mMixWith = animTwo->_renderList[i];
//		mMixWith->_visible = false;
//		mMixWith->PreCalcLocalPosition(positionTwo);
//
//		_partPositions[i].angle = mMixWith->precalculatedLocalPosition.angle * minus 
//									+ m->precalculatedLocalPosition.angle * mix;
//		_partPositions[i].sx = mMixWith->precalculatedLocalPosition.sx * minus 
//									+ m->precalculatedLocalPosition.sx * mix;
//		_partPositions[i].sy = mMixWith->precalculatedLocalPosition.sy * minus
//									+ m->precalculatedLocalPosition.sy * mix;
//		_partPositions[i].x = mMixWith->precalculatedLocalPosition.x * minus 
//									+ m->precalculatedLocalPosition.x * mix;
//		_partPositions[i].y = mMixWith->precalculatedLocalPosition.y * minus 
//									+ m->precalculatedLocalPosition.y * mix;
//		assert(fabs(_partPositions[i].angle) < 10);
//	}
//
//}

// смешиваем с новой анимацией без учета матрицы, 
// результат рисуем с учетом установленой матрицы
void AnimationState::DrawMixed(Animation *target, float position, float mix) {
	float minus = 1 - mix;
	for(unsigned int i = 0; i < target->_renderList.size();i++) {
		MovingPart *m = target->_renderList[i];
		m->_visible = false;
		m->PreCalcLocalPosition(position);
		m->localPosition.angle = _partPositions[i].angle * minus + m->precalculatedLocalPosition.angle * mix;
		m->localPosition.sx = _partPositions[i].sx * minus + m->precalculatedLocalPosition.sx * mix;
		m->localPosition.sy = _partPositions[i].sy * minus + m->precalculatedLocalPosition.sy * mix;
		m->localPosition.x = _partPositions[i].x * minus + m->precalculatedLocalPosition.x * mix;
		m->localPosition.y = _partPositions[i].y * minus + m->precalculatedLocalPosition.y * mix;
	}

	// для задания положения все спрайтов использую свой внутренний "стек" матриц
	Matrix matrix(target->_subPosition);
	for (unsigned int i = 0; i < target->_bones.size(); ++i) {
		target->_bones[i]->PreDrawInLocalPosition(matrix);
	}

	for(unsigned int i = 0; i < target->_renderList.size();i++) {
		target->_renderList[i]->Draw();
	}

	for(unsigned int i = 0; i < target->_renderList.size();i++) {
		target->_renderList[i]->SecondLayerDraw();
	}
}

// смешиваем с новой анимацией без учета матрицы, но вместо рисовния
// делаем ее новым слепком
bool AnimationState::MixWith(Animation *anim, float position, float mix, Animation *targ) {

	assert(_partPositions.size() == anim->_renderList.size());
	for (unsigned int i = 0; i < _partPositions.size(); ++i) {
		if (_partPositions[i].name != anim->_renderList[i]->boneName)
		{
			return false;
		}
		assert(_partPositions[i].name == anim->_renderList[i]->boneName);
	}

	float minus = 1 - mix;
	for(unsigned int i = 0; i < anim->_renderList.size();i++) {
		MovingPart *m = anim->_renderList[i];
		m->_visible = false;
		m->PreCalcLocalPosition(position);
		_partPositions[i].angle = _partPositions[i].angle * minus + m->precalculatedLocalPosition.angle * mix;
		_partPositions[i].sx = _partPositions[i].sx * minus + m->precalculatedLocalPosition.sx * mix;
		_partPositions[i].sy = _partPositions[i].sy * minus + m->precalculatedLocalPosition.sy * mix;
		_partPositions[i].x = _partPositions[i].x * minus + m->precalculatedLocalPosition.x * mix;
		_partPositions[i].y = _partPositions[i].y * minus + m->precalculatedLocalPosition.y * mix;
		if (fabs(_partPositions[i].angle) >= 10)
		{
			return false;
		}
		assert(fabs(_partPositions[i].angle) < 10);
	}

	if (_partPositions.size() != targ->_renderList.size())
	{
		return false;
	}
	assert(_partPositions.size() == targ->_renderList.size());
	std::map<std::string, PartPosition> bones;
	for (unsigned int i = 0; i < _partPositions.size(); ++i) {
		bones[_partPositions[i].name] = _partPositions[i];
	}
	for (unsigned int i = 0; i < targ->_renderList.size(); ++i) {
		_partPositions[i] = bones[targ->_renderList[i]->boneName];
		if (fabs(_partPositions[i].angle) >= 10)
		{
			return false;
		}
		assert(fabs(_partPositions[i].angle) < 10);
	}
	return true;
}

void Animation::DrawMixed(float position, const Animation &anim, float animPosition, float mix) {
	float minus = 1 - mix;
	for(unsigned int i = 0; i < _renderList.size();i++) {
		MovingPart *m = _renderList[i];
		m->_visible = false;
		m->PreCalcLocalPosition(position);

		MovingPart *mMixWith = anim._renderList[i];
		mMixWith->_visible = false;
		mMixWith->PreCalcLocalPosition(animPosition);

		m->localPosition.angle = mMixWith->precalculatedLocalPosition.angle * minus 
									+ m->precalculatedLocalPosition.angle * mix;
		m->localPosition.sx = mMixWith->precalculatedLocalPosition.sx * minus 
									+ m->precalculatedLocalPosition.sx * mix;
		m->localPosition.sy = mMixWith->precalculatedLocalPosition.sy * minus
									+ m->precalculatedLocalPosition.sy * mix;
		m->localPosition.x = mMixWith->precalculatedLocalPosition.x * minus 
									+ m->precalculatedLocalPosition.x * mix;
		m->localPosition.y = mMixWith->precalculatedLocalPosition.y * minus 
									+ m->precalculatedLocalPosition.y * mix;
	}

	// для задания положения все спрайтов использую свой внутренний "стек" матриц
	Matrix matrix(Render::GetCurrentMatrix());
	matrix.Mul(_subPosition);
	for (unsigned int i = 0; i < _bones.size(); ++i) {
		_bones[i]->PreDrawInLocalPosition(matrix);
	}

	for(unsigned int i = 0; i < _renderList.size();i++) {
		_renderList[i]->Draw();
	}
}



/* РЕАЛИЗАЦИЯ МЕТОДОВ МЕНЕДЖЕРА АНИАМАЦИЙ */

std::vector<std::string> animationPull;
bool AnimationManager::enableThread = false;
pthread_t threadAnimation;
pthread_mutex_t mutexAnimation;
pthread_mutex_t mutexGetAnimation;

static void *thread_animations(void *)
{
    std::string fileName;
    while (AnimationManager::EnableThread())
    {
		fileName.clear();
        pthread_mutex_lock(&mutexAnimation);
        if (animationPull.size())
        {
			fileName = animationPull.back();
            animationPull.pop_back();
        }
        pthread_mutex_unlock(&mutexAnimation);
		if (fileName.size())
		{
			AnimationManager::LoadInner(fileName);
		}
        else
            usleep(1);
    }
    return NULL;
}

void AnimationManager::FinishThread()
{
    if (enableThread)
    {   
		enableThread = false;
        pthread_join(threadAnimation, NULL);
        pthread_mutex_destroy(&mutexAnimation);
        pthread_mutex_destroy(&mutexGetAnimation);

		while (animationPull.size())
		{
			LoadInner(animationPull.back());
			animationPull.pop_back();
		}
    }
}

void AnimationManager::EnableThread(bool v)
{
	if (!enableThread && v)
	{
        pthread_create(&threadAnimation, NULL, thread_animations, NULL);
        pthread_mutex_init(&mutexAnimation, NULL);
        pthread_mutex_init(&mutexGetAnimation, NULL);
		enableThread = v;
	}
	else if (enableThread && !v)
	{
		FinishThread();
	}
}

void AnimationManager::ParseAnimationDescription(rapidxml::xml_node<> *root, const std::string &fileName, GLTexture *tex, int width, int height)
{
	rapidxml::xml_node<> *animation = root->first_node("Animation");
	while (animation)
	{
		const char *id = animation->first_attribute("id")->value();
		// ВНИМАНИЕ - из двух и более анимаций с одинаковыми id - будет загружена только первая
		bool existAnim;
		if (enableThread)
		{
			pthread_mutex_lock(&mutexGetAnimation);
			existAnim = (_animations.find(id) != _animations.end());
			pthread_mutex_unlock(&mutexGetAnimation);
		}
		else
			existAnim = (_animations.find(id) != _animations.end());
		if (!existAnim)
		{ 
			Animation *a = new Animation(id, animation, tex, width, height);
			a->fileName = fileName;
			if (enableThread)
			{
				pthread_mutex_lock(&mutexGetAnimation);
				_animations[id] = a;
				pthread_mutex_unlock(&mutexGetAnimation);
			}
			else
				_animations[id] = a;
		}
		else
		{
            log::write("animation already loaded: %s", id);
		}
		animation = animation->next_sibling("Animation");
	}
}
    
void AnimationManager::LoadInner(std::string fileName)
{
    //std::ifstream myfile(fileName.c_str());
    std::vector<char> buffer;
    Core::ReadFileToBuffer(fileName.c_str(), buffer);

    if (buffer.size())
    {
        rapidxml::xml_document<> doc;
        doc.parse<0>(&buffer[0]);
        
        // если файл существует - читаем XML с описанием анимаций
        rapidxml::xml_node<> *root = doc.first_node();
        //rapidxml::xml_node<> *params = root->first_node("params");
        GLTexture *tex = NULL;
        float width = 0.f;
        float height = 0.f;
        //if (params == NULL)
        {
            tex = Core::getTexture(fileName.substr(0, fileName.length() - 3) + "png");
            if (tex == 0)
            { // если текстура не найдена - прерываем загрузку, возвращаем "false"
                log::write("assigned png file not found %spng", (fileName.substr(0, fileName.length() - 3).c_str()));
                return;
            }
        }
//        else
//        {
//            width = (float)atof(params->first_attribute("width")->value());
//            height = (float)atof(params->first_attribute("height")->value());
//        }
        
        ParseAnimationDescription(root, fileName, tex, width, height);

        _files.insert(fileName);// запоминаем загруженый файл в списке
    }
    else 
    { 
        // файл анимаций не найден или его не удалось прочитать
        // выгружаем текстуру, возвращаем "false"
        
        Core::unloadTexture(fileName.substr(0, fileName.length() - 3) + "png");
        
        log::write("file not found: %", fileName.c_str());
    }
}

// Загрузить файл анимации
// в случае успеха - вернет "true"
// можно загружать несколько файлов - менеджер "коллекционирует" анимации
//
// здесь и далее используется HGE для загрузки и выгрузки текстур
// эти части кода нужно заменить при портировании на другие движки 
//
void AnimationManager::Load(std::string fileName, bool threadSafe)
{
	// загружаем текстуру - записываем указатель на нее в "tex", который используется далее
	//
	// начало блока - этот код при портировании надо переписать
	
    Math::Replace(fileName);

	// если этот файл уже загружен - ничего не делаем
	if (Loaded(fileName))
	{
		return;
	}

    if (EnableThread() && threadSafe)
    {
        pthread_mutex_lock(&mutexAnimation);
        animationPull.push_back(fileName);
        pthread_mutex_unlock(&mutexAnimation);
    }
    else
    {
        LoadInner(fileName);
    }
}

//
// Выгрузить все анимации и текстуры из памяти
//
void AnimationManager::UnloadAll()
{
	assert(!EnableThread());
	for (AnimationMap::iterator it = _animations.begin(); it != _animations.end(); it++) {
		delete (*it).second;
	}
	_animations.clear();
	for (std::set<std::string>::iterator it = _files.begin(); it != _files.end(); it++) {
		Core::unloadTexture((*it).substr(0, (*it).length() - 3) + "png");
	}
	_files.clear();
}

//
// Получить указатель на анимацию по animationId - это имя анимации в редакторе 
// (ВАЖНО нельзя вызывать delete для нее!)
//
Animation *AnimationManager::getAnimation(const std::string &animationId) 
{
	assert(!EnableThread());
	AnimationMap::iterator it_find = _animations.find(animationId);
	if (it_find != _animations.end())
	{
		if ((*it_find).second->_vb.tex == 0)
		{
            GLTexture *spr = Core::getTexture((*it_find).second->fileName);
			((*it_find).second)->ReloadTexture(spr);
		}
		return (*it_find).second;
	}
    log::write("animation %s not found.", animationId.c_str());
	return NULL;
}

AnimationManager::AnimationMap AnimationManager::_animations;

void AnimationManager::Unload(const std::string &fileName) 
{
	assert(!EnableThread());
	std::string fullFileName = fileName;
	if (fullFileName.find(".xml") != std::string::npos)// из-за конфликта с движком реалора пришлось файлы анимации переименовать - xml в aml
	{
		fullFileName.replace(fullFileName.find(".xml"), 4, ".aml");
	}

	Math::Replace(fullFileName);

    std::set<std::string>::iterator i = _files.find(fullFileName);
    _files.erase(i);

	Core::unloadTexture(fullFileName.substr(0, fullFileName.length() - 3) + "png");

	for (AnimationMap::iterator i = _animations.begin(); i != _animations.end(); )
	{
		if (i->second->fileName == fileName)
		{
			delete (i->second);
			i = _animations.erase(i);
		}
		else
		{
			++i;
		}
	}
}

bool AnimationManager::Loaded(const std::string &fileName)
{
    return _files.find(fileName) != _files.end();
}

void AnimationManager::ReplaceTexture(std::string fileName, GLTexture *spr)
{
	assert(!EnableThread());
	Math::Replace(fileName);
	fileName = fileName.substr(0, fileName.length() - 3) + "aml";
	if (!Loaded(fileName))
	{
		return;
	}
	for (AnimationMap::iterator i = _animations.begin(), e = _animations.end(); i != e; ++i)
	{
		if (i->second->fileName == fileName)
		{
			(i->second)->ReloadTexture(spr);
		}
	}
}

void AnimationManager::ReloadTextures()
{
	assert(!EnableThread());
	for (AnimationMap::iterator i = _animations.begin(), e = _animations.end(); i != e; ++i)
	{
		if (i->second->_vb.tex == 0)
		{
            GLTexture *spr = Core::getTexture(i->second->fileName);
			{
				(i->second)->ReloadTexture(spr);
			}
		}
	}
}
    
void AnimationManager::UploadTexture(const std::string &fileName)
{
    AnimationMap::iterator i = _animations.find(fileName);
    if (i != _animations.end())
    {
        GLTexture * tex = Core::getTexture(i->second->fileName);
        if (tex)
        {
            assert(false);
            //GLTexture *spr = tex->GetTexture();
            //spr->Touch();
        }
    }
}


};
