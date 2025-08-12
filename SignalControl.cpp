#include "SignalControl.h"

void Signal::initvar()
{
	dir = { {0,-1}, {-1,0}, {0,1}, {1,0} };
	height = 5.f;
	width = 14.f;
    pos = { {-width / 2.f, 0}, {0, -width / 2.f}, {-width / 2.f, -height}, {-height, -width / 2.f} };
	ifstream read("intersections.txt");
	float x, y;
	while (read >> x >> y) {
		points.emplace_back(x, y);
	}
    read.close();
    lightPos.resize(points.size());
    lightPos.assign(points.size(), vector<vector<Vector2f>> (4));
    cycle.resize(points.size());
    for (int k = 0;k < points.size();k++) {
        cycle[k].restart();
        for (int i = 0;i < 4;i+=2) {
            for (int j = 0;j < 3;j++) {
                lightPos[k][i].push_back({ (points[k] + pos[i] + Vector2f(dir[i].x * 20.f, dir[i].y * 20.f)).x + (width / 6.f) + (width / 3.f) * j, (points[k] + pos[i] + Vector2f(dir[i].x * 20.f, dir[i].y * 20.f)).y + height / 2.f });
            }
        } 
        for (int i = 1;i < 4;i += 2) {
            for (int j = 0;j < 3;j++) {
                lightPos[k][i].push_back({ (points[k] + pos[i] + Vector2f(dir[i].x * 20.f, dir[i].y * 20.f)).x + height / 2.f, (points[k] + pos[i] + Vector2f(dir[i].x * 20.f, dir[i].y * 20.f)).y + (width / 6.f) + (width / 3.f) * j });
            }
        }
    }
    for (int i = 0;i < 4;i++) {
        node* newnode = new node();
        newnode->val = i;
        newnode->next = nullptr;
        if (cycleVar == nullptr) {
            cycleVar = newnode;
        }   
        else {
            node* curr = cycleVar;
            while (curr->next != NULL) {
                curr = curr->next;
            }
            if (newnode->val == 3) newnode->next = cycleVar;
            curr->next = newnode;
        }
    }
    current = cycleVar;
    clr.resize(4);
    clr.assign(4, vector<Color>(3,{ Color::Green }));
    
}

Signal::Signal() : cycleVar(nullptr), current(nullptr)
{
	initvar();
}

Signal::~Signal()
{
    if (cycleVar == nullptr) return;
    node* tail = cycleVar;
    while (tail->next != cycleVar) {
        tail = tail->next;
    }
    tail->next = nullptr; 

    node* curr = cycleVar;
    while (curr != nullptr) {
        node* temp = curr;
        curr = curr->next;
        delete temp;
    }

    cycleVar = nullptr;
    current = nullptr;
}

void Signal::drawColors(RenderWindow& window, double RENDER_WORLD_SCALE_X, double RENDER_WORLD_SCALE_Y, float cx, float cy, Color color)
{
    float r = 2.f * (RENDER_WORLD_SCALE_X + RENDER_WORLD_SCALE_Y) / 2.f;
    CircleShape circle(r);
    circle.setPosition((cx * RENDER_WORLD_SCALE_X - r), (cy * RENDER_WORLD_SCALE_Y - r)); // center the circle at cx, cy
    circle.setFillColor(color);
    circle.setOutlineThickness(1.f);
    circle.setOutlineColor(Color(105, 105, 105));
    window.draw(circle);
}

void Signal::drawsignals(RenderWindow& window, double RENDER_WORLD_SCALE_X, double RENDER_WORLD_SCALE_Y)
{
    View view = window.getView();
    FloatRect viewBounds(view.getCenter() - view.getSize() / 2.f, view.getSize());

    Color red(150, 0, 0);
    Color orange(150, 65, 0);
    Color green(0, 150, 0);
    Color darkGray(105, 105, 105);
    Color black(0, 0, 0);

    vector<Color> colors(3);
    colors = { red,orange,green };

    // Helper lambda to draw a signal housing (rectangle)
    auto drawHousing = [&](Vector2f leftTop, float width, float height) {
        RectangleShape housing(Vector2f(width * RENDER_WORLD_SCALE_X, height * RENDER_WORLD_SCALE_Y));
        housing.setPosition(leftTop.x * RENDER_WORLD_SCALE_X, leftTop.y * RENDER_WORLD_SCALE_Y);
        housing.setFillColor(black);
        window.draw(housing);

        // Draw housing border with dark gray
        housing.setFillColor(Color::Transparent);
        housing.setOutlineThickness(0.2f);
        housing.setOutlineColor(darkGray);
        window.draw(housing);
        };

    // Top & Bottom signals
    for (int k = 0;k < points.size();k++) {
        FloatRect signalBounds(
            (points[k].x - 20.f) * RENDER_WORLD_SCALE_X,
            (points[k].y - 20.f) * RENDER_WORLD_SCALE_Y,
            40.f * RENDER_WORLD_SCALE_X,
            40.f * RENDER_WORLD_SCALE_Y
        );

        // If the signal's bounding box doesn't intersect with the view, skip drawing it
        if (!viewBounds.intersects(signalBounds)) {
            continue;
        }
        for (int j = 0;j < 4;j++) {
            drawHousing(points[k] + pos[j] + Vector2f(dir[j].x * 20.f, dir[j].y * 20.f), (j % 2) ? height : width, (j % 2) ? width : height);
        }
        for (int i = 0;i < 4;i += 2) {
            for (int j = 0;j < 3;j++) {
                drawColors(window, RENDER_WORLD_SCALE_X, RENDER_WORLD_SCALE_Y, lightPos[k][i][j].x, lightPos[k][i][j].y, colors[j]);
            }
        }
        for (int i = 1;i < 4;i += 2) {
            for (int j = 0;j < 3;j++) {
                drawColors(window, RENDER_WORLD_SCALE_X, RENDER_WORLD_SCALE_Y, lightPos[k][i][j].x, lightPos[k][i][j].y, colors[j]);
            }
        }
    }
}

void Signal::signalctrl(RenderWindow& window, double RENDER_WORLD_SCALE_X, double RENDER_WORLD_SCALE_Y)
{
    for (int k = 0;k < points.size();k++) {
        View view = window.getView();
        FloatRect viewBounds(view.getCenter() - view.getSize() / 2.f, view.getSize());
        FloatRect signalBounds(
            (points[k].x - 20.f) * RENDER_WORLD_SCALE_X,
            (points[k].y - 20.f) * RENDER_WORLD_SCALE_Y,
            40.f * RENDER_WORLD_SCALE_X,
            40.f * RENDER_WORLD_SCALE_Y
        );
       
        if (cycle[k].getElapsedTime().asSeconds() >= 15) {
            for (int i = 0;i < 4;i++) {
                if (i == current->val) clr[i] = { Color::Red, Color::Transparent, Color::Transparent };
                else clr[i] = { Color::Transparent, Color::Transparent, Color::Green };
            }
            current = current->next;
            cycle[k].restart();
        }
        if (!viewBounds.intersects(signalBounds)) continue;
        for (int i = 0;i < 4;i++) {
            for (int j = 0;j < 3;j++) {
                drawColors(window, RENDER_WORLD_SCALE_X, RENDER_WORLD_SCALE_Y, lightPos[k][i][j].x, lightPos[k][i][j].y, clr[i][j]);
            }
        }
    }
}
