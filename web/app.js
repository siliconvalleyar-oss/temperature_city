/* Weather dashboard — fetches /api/weather and renders city cards */

const grid = document.getElementById('grid');
const searchInput = document.getElementById('search');
const refreshBtn = document.getElementById('btn-refresh');
const statCount = document.getElementById('stat-count');
const statMin = document.getElementById('stat-min');
const statMax = document.getElementById('stat-max');
const statAvg = document.getElementById('stat-avg');
const statTime = document.getElementById('stat-time');

let allData = [];

/* Weather code -> emoji mapping (WMO codes) */
function weatherEmoji(code) {
  if (code === 0) return '☀️';
  if (code <= 3) return '⛅';
  if (code <= 48) return '🌫️';
  if (code <= 57) return '🌦️';
  if (code <= 67) return '🌧️';
  if (code <= 77) return '🌨️';
  if (code <= 82) return '🌦️';
  if (code <= 86) return '🌨️';
  return '☁️';
}

/* Temperature color class */
function tempClass(t) {
  if (t <= 0) return 'card-freezing';
  if (t <= 10) return 'card-cold';
  if (t <= 20) return 'card-mild';
  if (t <= 30) return 'card-warm';
  if (t <= 40) return 'card-hot';
  return 'card-very-hot';
}

function render(cities) {
  grid.innerHTML = '';

  if (cities.length === 0) {
    grid.innerHTML = '<div class="loading">No se encontraron ciudades</div>';
    return;
  }

  let valid = cities.filter(c => c.temp !== undefined);
  let temps = valid.map(c => c.temp);
  let min = temps.length ? Math.min(...temps) : null;
  let max = temps.length ? Math.max(...temps) : null;
  let avg = temps.length ? (temps.reduce((a, b) => a + b, 0) / temps.length).toFixed(1) : null;

  statCount.textContent = valid.length;
  statMin.textContent = min !== null ? `${min.toFixed(1)}°C` : '--°C';
  statMax.textContent = max !== null ? `${max.toFixed(1)}°C` : '--°C';
  statAvg.textContent = avg !== null ? `${avg}°C` : '--°C';
  statTime.textContent = new Date().toLocaleTimeString('es-AR', { hour: '2-digit', minute: '2-digit' });

  cities.forEach(c => {
    const card = document.createElement('div');
    card.className = 'card';

    if (c.temp !== undefined) {
      card.classList.add(tempClass(c.temp));
      card.innerHTML = `
        <div class="card-name">${weatherEmoji(c.weather_code)} ${c.name}</div>
        <div class="card-temp">${c.temp.toFixed(1)}°C</div>
        <div class="card-feels">Sensación térmica: ${c.feels_like.toFixed(1)}°C</div>
        <div class="card-details">
          <span class="detail"><span class="icon">📈</span> ${c.temp_max.toFixed(1)}°C</span>
          <span class="detail"><span class="icon">📉</span> ${c.temp_min.toFixed(1)}°C</span>
          <span class="detail"><span class="icon">💧</span> ${c.humidity}%</span>
          <span class="detail"><span class="icon">💨</span> ${c.wind.toFixed(0)} km/h</span>
        </div>
      `;
    } else {
      card.innerHTML = `
        <div class="card-name">⚠️ ${c.name}</div>
        <div class="card-error">Error: ${c.error || 'sin datos'}</div>
      `;
    }

    grid.appendChild(card);
  });
}

function filter(query) {
  if (!query) return render(allData);
  const q = query.toLowerCase();
  render(allData.filter(c => c.name.toLowerCase().includes(q)));
}

async function fetchData() {
  grid.innerHTML = '<div class="loading">Cargando datos climáticos...</div>';
  try {
    const resp = await fetch('/api/weather');
    allData = await resp.json();
    render(allData);
  } catch (err) {
    grid.innerHTML = `<div class="loading">Error al conectar: ${err.message}</div>`;
  }
}

/* Events */
searchInput.addEventListener('input', e => filter(e.target.value));
refreshBtn.addEventListener('click', fetchData);

/* Auto-refresh every 5 minutes */
fetchData();
setInterval(fetchData, 300000);
