#include <iostream>
#include <fstream>
#include <memory>
#include <ostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class Pokemon{
    private:
        std::string nombre;
        int experiencia;

    public:
        Pokemon(const std::string& nombre, int experiencia)
            : nombre(nombre), experiencia(experiencia) {}
        
        std::string getNombre() const {
            return nombre;
        }

        int getExperiencia() const {
            return experiencia;
        }

        bool operator==(const Pokemon& other) const {
            return nombre == other.nombre;
        }
};

struct hashPokemon{
    std::size_t operator()(const Pokemon& pokemon) const {
        return std::hash<std::string>()(pokemon.getNombre());
    }
};

class PokemonInfo{
    private:
        std::string tipo;
        std::string descripcion;
        std::vector<std::pair<std::string, int>> ataquesDisponiblesPorNivel;
        std::vector<int> experienciaProximoNivel;

    public:
        PokemonInfo() = default;
        PokemonInfo(const std::string& tipo, const std::string& descripcion,
                    std::vector<std::pair<std::string, int>> ataquesDisponiblesPorNivel,
                    std::vector<int> experienciaProximoNivel)
            : tipo(tipo), descripcion(descripcion),
              ataquesDisponiblesPorNivel(std::move(ataquesDisponiblesPorNivel)),
              experienciaProximoNivel(std::move(experienciaProximoNivel)) {}
        
        std::string getTipo() const {
            return tipo;
        }

        std::string getDescripcion() const {
            return descripcion;
        }

        const std::vector<std::pair<std::string, int>>& getAtaquesDisponiblesPorNivel() const {
            return ataquesDisponiblesPorNivel;
        }

        const std::vector<int>& getExperienciaProximoNivel() const {
            return experienciaProximoNivel;
        }

};

class Pokedex{
    private:
        std::unordered_map<Pokemon, PokemonInfo, hashPokemon> pokedex;
        std::string nombreArchivo;

    public:
        Pokedex(const std::string& nombreArchivo = "pokedex.txt")
            : nombreArchivo(nombreArchivo) {
            cargarDesdeArchivo();
        }

        void agregarPokemon(const Pokemon& pokemon, const PokemonInfo& info) {
            pokedex[pokemon] = info;
            guardarEnArchivo(this->nombreArchivo);
        }

        void cargarDesdeArchivo() {
            std::ifstream archivo(this -> nombreArchivo);
            if (!archivo.is_open()) {
                std::cerr << "Error al abrir el archivo: " << this -> nombreArchivo << "\n";
                return;
            }

            std::string linea;
            while (std::getline(archivo, linea)) {
                std::string nombre, tipo, descripcion;
                int experiencia;

                std::stringstream ss(linea);

                std::getline(ss, nombre, ',');
                std::string experienciaStr;
                std::getline(ss, experienciaStr, ',');
                experiencia = std::stoi(experienciaStr);
                std::getline(ss, tipo, ',');
                std::getline(ss, descripcion, ',');

                std::vector<std::pair<std::string, int>> ataquesDisponiblesPorNivel;
                while (std::getline(archivo, linea) && !linea.empty()) {
                    if (linea.find(',') != std::string::npos) {
                    std::stringstream ataqueStream(linea);
                    std::string ataqueNombre, danoStr;
                    std::getline(ataqueStream, ataqueNombre, ',');
                    std::getline(ataqueStream, danoStr);
                    ataquesDisponiblesPorNivel.emplace_back(ataqueNombre, std::stoi(danoStr));
                    } else {
                        break;
                        }
                }

                std::vector<int> experienciaProximoNivel;
                for (int i = 0; i < 3; ++i) {
                    if (std::getline(archivo, linea)) {
                        experienciaProximoNivel.push_back(std::stoi(linea));
                    }
                }

                Pokemon pokemon(nombre, experiencia);
                PokemonInfo infoPokemon(tipo, descripcion, ataquesDisponiblesPorNivel, experienciaProximoNivel);
                pokedex[pokemon] = infoPokemon;
            }  
            archivo.close();
        }

        void guardarEnArchivo(const std::string& nombreArchivo) const {
            std::ofstream archivo(nombreArchivo);
            if (!archivo.is_open()) {
                std::cerr << "Error al abrir el archivo: " << nombreArchivo << "\n";
                return;
            }

            for (const auto& entry : pokedex) {
                const Pokemon& pokemon = entry.first;
                const PokemonInfo& info = entry.second;

                archivo << pokemon.getNombre() << "," 
                        << pokemon.getExperiencia() << ","
                        << info.getTipo() << ","
                        << info.getDescripcion() << "\n";

                for (const auto& ataque : info.getAtaquesDisponiblesPorNivel()) {
                    archivo << ataque.first << "," << ataque.second << "\n";
                }

                for (int exp : info.getExperienciaProximoNivel()) {
                    archivo << exp << "\n";
                }

                archivo << "\n"; // Separador entre pokemons
            }
            archivo.close();
        }

        void mostrar(const Pokemon& pokemon) const {
            auto it = pokedex.find(pokemon);
            if (it != pokedex.end()) {
                const PokemonInfo& info = it->second;
                std::cout << "Nombre: " << pokemon.getNombre() << "\n";
                std::cout << "Experiencia: " << pokemon.getExperiencia() << "\n";
                std::cout << "Tipo: " << info.getTipo() << "\n";
                std::cout << "Descripcion: " << info.getDescripcion() << "\n";
                std::cout << "Ataques disponibles por nivel:\n";
                for (const auto& ataque : info.getAtaquesDisponiblesPorNivel()) {
                    std::cout << "- " << ataque.first << " (Daño: " << ataque.second << ")\n";
                }
                std::cout << "Experiencia para el proximo nivel:\n";
                for (int exp : info.getExperienciaProximoNivel()) {
                    std::cout << "- " << exp << "\n";
                }
            } else {
                std::cout << "“¡Pokemon desconocido!\n";
            }
        }

        void mostrarTodos() const {
            for (const auto& entry : pokedex) {
                mostrar(entry.first);
                std::cout << "------------------------\n";
            }
        }
};

int main(){
    Pokedex pokedex("pokedex.txt");
    Pokemon squirtle("Squirtle", 100);
    PokemonInfo infoSquirtle("Agua", "Una tortuga pequeña que lanza chorros de agua.",
                             {{"Pistola Agua", 4}, {"Hidrobomba", 6}, {"Danza Lluvia", 5}},
                             {0, 400, 1000});
    pokedex.agregarPokemon(squirtle, infoSquirtle);

    Pokemon bulbasaur("Bulbasaur", 270);
    PokemonInfo infoBulbasaur("Planta", "Tiene una semilla en su lomo que crece con el tiempo",
                              {{"Latigazo", 4}, {"Hoja Afilada", 6}, {"Rayo Solar", 5}},
                              {0, 300, 1100});

    pokedex.agregarPokemon(bulbasaur, infoBulbasaur);

    Pokemon charmander("Charmander", 633);
    PokemonInfo infoCharmander("Fuego", "Una lagartija con una llama en su cola",
                               {{"Ascuas", 4}, {"Lanzallamas", 6}, {"Giro Fuego", 5}},
                               {0, 250, 1300});

    pokedex.agregarPokemon(charmander, infoCharmander);

    pokedex.mostrarTodos();
}